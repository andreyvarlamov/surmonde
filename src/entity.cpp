#include "entity.h"
#include "sav.h"
#include "helpers.h"
#include "defines.h"
#include "draw.h"
#include "navigation.h"

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, SavTextureAtlas *atlas, f32 tilePxW, f32 tilePxH)
{
    EntityStore s = {};
    s.entityMax = entityMax;
    s.entities = MemoryArenaPushArrayAndZero(arena, entityMax, Entity);

    // s.w = spatialW;
    // s.h = spatialH;
    // s.spatialEntities = MemoryArenaPushArrayAndZero(arena, spatialW * spatialH, Entity *);

    s.arena = arena;
    s.atlas = atlas;
    s.tilePxW = tilePxW;
    s.tilePxH = tilePxH;

    return s;
}

api_func Entity MakeEntity(f32 x, f32 y, Level *level, i32 atlasValue, SavColor bg, SavColor fg)
{
    Entity e = {};
    e.p = V2(x, y);
    e.level = level;
    e.atlasValue = atlasValue;
    e.bg = bg;
    e.fg = fg;
    e.isUsed = true;
    return e;
}

api_func void ConfigureCharacterEntity(Entity *e, CharacterStats stats)
{
    e->stats = stats;
    e->stats.isConfigured = true;
}

api_func Entity *AddEntity(EntityStore *s, Entity e)
{
    // TODO: Slot reuse
    Assert(s->entityCount < s->entityMax);
    Entity *entityInStore = s->entities + s->entityCount++;
    *entityInStore = e;
    // addSpatialEntity(s, entityInStore);
    return entityInStore;
}

internal_func void recalculatePath(Entity *e, MemoryArena *arena)
{
    Assert(e->brain.isOrderedMovement);

    TraceLogPartialStart("Recalculating path... ");
    
    MemoryArenaFreeze(arena);
    NavPath path = NavPathToTarget(e->level, e->p, e->brain.movementTarget, arena);
    if (path.found)
    {
        if (path.alreadyAtTarget)
        {
            e->brain.isOrderedMovement = false;
            TraceLogPartial("Arrived at target.\n");
        }
        else
        {
            e->brain.pathNextTarget = path.path[0];
            TraceLogPartial("Next target: %.2f, %.2f\n", e->brain.pathNextTarget.x, e->brain.pathNextTarget.y);
        }
    }
    else
    {
        e->brain.isOrderedMovement = false;
        TraceLogPartial("Target unreachable.\n");
    }
    MemoryArenaUnfreeze(arena);
}

api_func void OrderEntityMovement(EntityStore *s, Entity *e, v2 target)
{
    e->brain.isOrderedMovement = true;
    e->brain.movementTarget = target;

    recalculatePath(e, s->arena);
}

api_func void OrderFollowEntity(Entity *follower, Entity *followed)
{
    follower->brain.isOrderedFollow = true;
    follower->brain.followedEntity = followed;
}

internal_func inline b32 isEntityInRange(Entity *e, Entity *entityToCheck, f32 radius)
{
    b32 result = VecLengthSq(entityToCheck->p - e->p) < Square(radius);
    return result;
}

api_func void InitiateCombat(Entity *attacker, Entity *defender)
{
    attacker->brain.combatState = COMBAT_STATE_READY;
    attacker->brain.opponent = defender;
    // attacker->brain.combatTimer = COMBAT_ROUND_DURATION;
    attacker->brain.combatStamina = 100.0f;

    defender->brain.isOrderedMovement = false;
    defender->brain.isOrderedFollow = false;
    defender->brain.combatState = COMBAT_STATE_READY;
    defender->brain.opponent = attacker;
    defender->brain.combatStamina = 100.0f;
    // defender->brain.combatTimer = COMBAT_ROUND_DURATION;

    TraceLog("%p is initiating combat with %p.", attacker, defender);
}

internal_func inline f32 getRotationSpeed(Entity *e)
{
    // TODO: Revisit this. Idk if it even makes sense to scale rotation speed with stats
    return e->stats.speed / ENTITY_BASE_SPEED * ENTITY_BASE_ROTATION_SPEED;
}

internal_func b32 moveEntity(Entity *e, v2 target, f32 delta)
{
    v2 movement = target - e->p;
    f32 dist = VecLength(movement);
    if (dist < CMP_EPSILON)
    {
        e->p = target;
        return true;
    }

    v2 dir = movement / dist;
    f32 angle = ToDeg(GetVecFlippedYAngle(dir));

    f32 moveAmount;
    if (AbsF32(GetAngleDegDiff(e->yawDeg, angle)) > QUICK_TURN_AROUND_MAX)
    {    
       	moveAmount = e->stats.speed * 0.2f * delta;
    }
    else
    {
        moveAmount = e->stats.speed * delta;
    }

    MoveTowardAngleDeg(&e->yawDeg, angle, getRotationSpeed(e) * delta);
    b32 targetReached = MoveToward(&e->p, target, moveAmount);

    return targetReached;
}

#if 0
internal_func b32 pathEntity(Entity *e, v2 target, f32 delta)
{
    v2 nextStep = NavPathToTarget(e->level, e->p, target);
    moveEntity(e, nextStep, delta);
}
#endif

internal_func inline void setEntityCombatState(Entity *e, CombatState state, f32 howLong)
{
    e->brain.combatState = state;
    e->brain.combatTimer = howLong;
    TraceLog("%p combat state: %d for %.1f s", e, state, howLong);
}

internal_func inline b32 performAttackHit(Entity *attacker, Entity *defender)
{
    b32 attackerHits = RandomChance(80.0f);
    b32 defenderBlocks = defender->brain.combatState == COMBAT_STATE_DEFENCE_MID && RandomChance(80.0f);

    attacker->brain.combatStamina -= 20.0f;
    if (defenderBlocks)
    {
        defender->brain.combatStamina -= 2.0f;
    }

    if (!attackerHits)
    {
        AddFloater("Miss", 20, SAV_COLOR_YELLOW, attacker->p);
    }
    else if (!defenderBlocks)
    {
        AddFloater(TextFormat("-%.0f", 20.0f), 20, SAV_COLOR_RED, attacker->p);
    }
    else
    {
        AddFloater("Deflect", 20, SAV_COLOR_GREEN, defender->p);
    }

    return attackerHits && !defenderBlocks;
}

internal_func void processCharacterAI(EntityStore *s, Entity *e)
{
    Assert(e->stats.isConfigured);

    if (e->brain.combatState == COMBAT_STATE_NONE && isEntityInRange(e, s->controlledEntity, e->stats.combatRadius))
    {
        e->brain.isOrderedFollow = false;
        e->brain.isOrderedMovement = false;
        InitiateCombat(e, s->controlledEntity);
    }
    else if (!e->brain.isOrderedFollow && isEntityInRange(e, s->controlledEntity, e->stats.viewRadius))
    {
        // TODO: Check if in line of sight
        e->brain.isOrderedMovement = false;
        OrderFollowEntity(e, s->controlledEntity);
    }
    else if (!e->brain.isOrderedFollow && !e->brain.isOrderedMovement)
    {
        if (GetRandomValue(0, 100) == 0)
        {
            OrderEntityMovement(s, e, e->p + GetRandomVec(2.0f + GetRandomFloat() * 5.0f));
        }
    }
}

internal_func void recoverStamina(f32 *stamina, f32 rate, f32 delta)
{
    *stamina += rate * delta;
    if (*stamina > 100.0f)
    {
        *stamina = 100.0f;
    }
}

internal_func void faceEntity(Entity *e, Entity *targetEntity, f32 delta)
{
    v2 dir = VecNormalize(targetEntity->p - e->p);
    f32 angle = ToDeg(GetVecFlippedYAngle(dir));
    // MoveTowardAngleDeg(&e->yawDeg, angle, getRotationSpeed(e) * delta);
    MoveTowardAngleDegDamped(&e->yawDeg, angle, 0.1f, getRotationSpeed(e) * delta);
}

internal_func void stayCloseToEntity(Entity *e, Entity *targetEntity, f32 delta)
{
    v2 dir = VecNormalize(e->p - targetEntity->p);
    v2 target = targetEntity->p + e->stats.combatRadius * dir;
    // target = e->p + 0.1f * (target - e->p); // TODO: ??
    // MoveToward(&e->p, target, e->stats.speed * delta);
    MoveTowardDamped(&e->p, target, 0.1f, e->stats.speed * delta);
}

internal_func void processCombatState(Entity *e, f32 delta)
{
    
    switch (e->brain.combatState)
    {
        case COMBAT_STATE_READY:
        {
            recoverStamina(&e->brain.combatStamina, 20.0f, delta);

            // TODO: Take diff of initiatives. And scale the sidestepping chance.
            //       An entity with a negative initiative diff, should have a very high chance of sidestepping only
            //       An entity with a positive intitiative diff, should have a very high chance of attacking.
            //       That means the decision of who goes in for an attack is made by each entity, and not centrally
                                
            if (AdvanceTimer(&e->brain.sidestepTimer, 0.5f, delta))
            {
                if (RandomChance(80.0f))
                {
                    v2 dir = VecNormalize(e->brain.opponent->p - e->p);
                    v2 perp = V2(-dir.y, dir.x);
                    f32 sign = GetRandomValue(0, 2) ? 1.0f : -1.0f;
                    e->brain.tempTarget = e->p + sign * (1.0f + 5.0f * GetRandomFloat()) * perp;
                    e->brain.isMovingInCombat = true;
                }
                else
                {
                    if (e->brain.combatStamina > 0.0f)
                    {
                        setEntityCombatState(e, COMBAT_STATE_ATTACK_PRE, 1.0f);
                    }
                }
                
                ResetTimer(&e->brain.sidestepTimer);
            }

            if (e->brain.isMovingInCombat)
            {
                if (MoveTowardDamped(&e->p, e->brain.tempTarget, 0.3f, e->stats.speed * delta))
                {
                    e->brain.isMovingInCombat = false;
                }
            }
            
            faceEntity(e, e->brain.opponent, delta);
            stayCloseToEntity(e, e->brain.opponent, delta);
            
            #if 1
            if (e->brain.opponent->brain.combatState == COMBAT_STATE_ATTACK_PRE)
            {
                setEntityCombatState(e, COMBAT_STATE_DEFENCE_PRE, 0.5f);
            }
            #endif
        } break;

        case COMBAT_STATE_ATTACK_PRE:
        {
            v2 dir = VecNormalize(e->p - e->brain.opponent->p);
            v2 target = e->brain.opponent->p + e->stats.attackReach * dir;
            
            if (MoveTowardDamped(&e->p, target, 0.3f, e->stats.speed * delta))
            
            // if (AdvanceTimer(&e->brain.combatTimer, delta))
            {
                setEntityCombatState(e, COMBAT_STATE_ATTACK_MID, 0.2f);
            }
        } break;

        case COMBAT_STATE_ATTACK_MID:
        {
            if (AdvanceTimer(&e->brain.combatTimer, delta))
            {
                performAttackHit(e, e->brain.opponent);

                if (e->brain.combatStamina > 0.0f)
                {
                    e->brain.combatTimer = 0.2f;
                }
                else
                {
                    setEntityCombatState(e, COMBAT_STATE_ATTACK_POST, 1.0f);
                }
            }
        } break;

        case COMBAT_STATE_ATTACK_POST:
        {
            v2 dir = VecNormalize(e->p - e->brain.opponent->p);
            v2 target = e->brain.opponent->p + e->stats.combatRadius * dir;
            
            if (MoveTowardDamped(&e->p, target, 0.3f, e->stats.speed * delta))

            // if (AdvanceTimer(&e->brain.combatTimer, delta))
            {
                setEntityCombatState(e, COMBAT_STATE_READY, 0.0f);
            }
        } break;
            
        case COMBAT_STATE_DEFENCE_PRE:
        {
            if (AdvanceTimer(&e->brain.combatTimer, delta))
            {
                setEntityCombatState(e, COMBAT_STATE_DEFENCE_MID, 0.0f);
            }
        } break;

        case COMBAT_STATE_DEFENCE_MID:
        {
            recoverStamina(&e->brain.combatStamina, 15.0f, delta);
            
            faceEntity(e, e->brain.opponent, delta);
            
            if (e->brain.opponent->brain.combatState == COMBAT_STATE_ATTACK_POST)// && e->brain.opponent->brain.combatStamina < 0.0f)
            {
                setEntityCombatState(e, COMBAT_STATE_DEFENCE_POST, 0.5f);
                
                // TODO: if opponent's post attack is quick, don't switch from defence
                //       if slow, defence post, then attack pre
                //       if chance, quick counter attack -- pre and post
                //       if counter attack succeeds, can follow up with normal attack
                //
                //       attack interruption
                //       working with stamina
                //
                //       lookahead state (e.g. defence post now, with intention to start attacking after)
                // 
                //       attack speed, entity estimating the speed of enemy's attack pre -- can maybe interrupt with a short attack (check enemy's combat timer + some rng)
                //       having a choice of attacks available to you based on skills
            }
        } break;

        case COMBAT_STATE_DEFENCE_POST:
        {
            if (AdvanceTimer(&e->brain.combatTimer, delta))
            {
                setEntityCombatState(e, COMBAT_STATE_READY, 0.0f);
            }
        } break;

        default: break;
    }
}

internal_func void processCharacterOrders(EntityStore *s, Entity *e, f32 delta)
{
    Assert(e->stats.isConfigured);

    if (e->brain.combatState != COMBAT_STATE_NONE)
    {
        processCombatState(e, delta);
    }
    else if (e->brain.isOrderedFollow)
    {
        moveEntity(e, e->brain.followedEntity->p, delta);
    }
    else if (e->brain.isOrderedMovement)
    {
        // TraceLog("Moving towards: %.3f, %.3f", e->brain.pathNextTarget.x, e->brain.pathNextTarget.y);
        if (moveEntity(e, e->brain.pathNextTarget, delta))
        {
            recalculatePath(e, s->arena);
        }
    }
}

api_func void UpdateEntities(EntityStore *s, f32 delta)
{
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        if (!IsControlledEntity(s, e))
        {
            processCharacterAI(s, e);
        }

        processCharacterOrders(s, e, delta);
    }
}

api_func void DrawEntities(EntityStore *s)
{
    int vertCount = s->entityCount * 4;
    int indexCount = s->entityCount * 6;

    MemoryArenaFreeze(s->arena);

    v3 *positions = MemoryArenaPushArrayAndZero(s->arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
    v4 *colorsBg = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
    v4 *colorsFg = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(s->arena, indexCount, u32);

    int vertsAdded = 0;
    int indicesAdded = 0;

    f32 halfTilePxW = s->tilePxW * 0.5f;
    f32 halfTilePxH = s->tilePxH * 0.5f;
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        f32 pxX = e->p.x * s->tilePxW - halfTilePxW;
        f32 pxY = e->p.y * s->tilePxH - halfTilePxH;
        Rect destRect = MakeRect(pxX, pxY, s->tilePxW, s->tilePxH);
        FourV3 points = ConvertFourV2V3(RectGetPoints(destRect));

        Rect atlasRect = GetAtlasSourceRect(*s->atlas, e->atlasValue);
        FourV4 texCoordPoints = ConvertFourV2V4(GetTextureRectTexCoords(s->atlas->texture, atlasRect));

        SavColor bg = {};

        if (e->brain.combatState != COMBAT_STATE_NONE)
        {
            bg = ColorAlpha(SAV_COLOR_CRIMSON, 0.2f);
        }

        u32 indexBase = vertsAdded;
        for (int i = 0; i < 4; i++)
        {
            positions[vertsAdded] = points.e[i];
            texCoords[vertsAdded] = texCoordPoints.e[i];
            colorsBg[vertsAdded] = GetColorV4(bg);
            colorsFg[vertsAdded] = GetColorV4(e->fg);
            vertsAdded++;
        }

        u32 localIndices[] = {0, 1, 2, 2, 3, 0};
        for (int i = 0; i < ArrayCount(localIndices); i++)
        {
            indices[indicesAdded++] = indexBase + localIndices[i];
        }
    }

    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords, vertCount, sizeof(texCoords[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colorsBg, vertCount, sizeof(colorsBg[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    
    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords, vertCount, sizeof(texCoords[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colorsFg, vertCount, sizeof(colorsFg[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    BindTextureSlot(0, s->atlas->texture);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    UnbindTextureSlot(0);

    MemoryArenaUnfreeze(s->arena);

#if (DRAW_ENTITY_DEBUG == 1)
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        if (!IsControlledEntity(s, e))
        {
            v2 pxP = V2(e->p.x * s->tilePxW, e->p.y * s->tilePxH);
            // NOTE: This visualization wouldn't work with non-square tiles.
            DrawFilledCircle(pxP, e->stats.viewRadius * s->tilePxW, ColorAlpha(SAV_COLOR_GRAY, 0.1f), s->arena);
        }

        v2 pxP = V2(e->p.x * s->tilePxW, e->p.y * s->tilePxH);
        DrawFilledCircleSegment(pxP, 64.0f, e->yawDeg - 10.0f, e->yawDeg + 10.0f, ColorAlpha(SAV_COLOR_YELLOW, 0.1f), s->arena);

        if (e->brain.isOrderedMovement)
        {
            v2 targetPxP = V2(e->brain.movementTarget.x * s->tilePxW, e->brain.movementTarget.y * s->tilePxH);
            DrawBadLine(pxP, targetPxP, SAV_COLOR_DARKSLATEGRAY);

            #if 0
            MemoryArenaFreeze(s->arena);
            NavPath path = NavPathToTarget(e->level, e->p, e->brain.movementTarget, s->arena);
            if (path.found)
            {
                for (int i = 0; i < path.nodeCount - 1; i++)
                {
                    v2 nodeA = path.path[i];
                    v2 nodeB = path.path[i + 1];
                    
                    v2 prevPxP = V2(nodeA.x * s->tilePxW, nodeA.y * s->tilePxH);
                    v2 currPxP = V2(nodeB.x * s->tilePxW, nodeB.y * s->tilePxH);
                    DrawLine(prevPxP, currPxP, SAV_COLOR_DARKSLATEGRAY);
                }
            }
            MemoryArenaUnfreeze(s->arena);
            #endif
        }
    }
#endif
}

internal_func inline const char *getCombatStateString(CombatState combatState)
{
    // TODO: Haha
    switch (combatState)
    {
        case COMBAT_STATE_NONE: return "None";
        case COMBAT_STATE_READY: return "Ready";
        case COMBAT_STATE_ATTACK_PRE: return "Pre Attack";
        case COMBAT_STATE_ATTACK_MID: return "Mid Attack";
        case COMBAT_STATE_ATTACK_POST: return "Post Attack";
        case COMBAT_STATE_DEFENCE_PRE: return "Pre Defence";
        case COMBAT_STATE_DEFENCE_MID: return "Mid Defence";
        case COMBAT_STATE_DEFENCE_POST: return "Post Defence";
        default: return "Unknown";
    }
}

api_func void DrawEntityUI(EntityStore *s, SavFont *font, MemoryArena *arena)
{
    f32 widgetX = 10.0f;
    f32 widgetY = 10.0f;
    f32 widgetWidth = 350.0f;
    f32 widgetHeight = 140.0f;
    f32 widgetPad = 5.0f;
    f32 widgetMargin = 5.0f;
    
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        if (e->brain.combatState != COMBAT_STATE_NONE)
        {
            DrawRect(MakeRect(widgetX, widgetY, widgetWidth, widgetHeight), SAV_COLOR_DARKDARKGRAY);

            f32 lineHeight = 30.0f;
            
            f32 inWidgetX = widgetX + widgetPad;
            f32 inWidgetY = widgetY + widgetPad;
            DrawString(TextFormat("Entity %p", e), font, 30, SAV_COLOR_WHITE, inWidgetX, inWidgetY, 0.0f, arena);

            inWidgetY += lineHeight;
            DrawString(TextFormat("Pos: %.3f, %.3f", e->p.x, e->p.y), font, 30, SAV_COLOR_WHITE, inWidgetX, inWidgetY, 0.0f, arena);
            
            inWidgetY += lineHeight;
            DrawString(TextFormat("Combat state: %s", getCombatStateString(e->brain.combatState)), font, 30, SAV_COLOR_WHITE, inWidgetX, inWidgetY, 0.0f, arena);

            inWidgetY += lineHeight;
            DrawString(TextFormat("Stamina: %.1f", e->brain.combatStamina), font, 30, SAV_COLOR_WHITE, inWidgetX, inWidgetY, 0.0f, arena);

            widgetY += widgetHeight + widgetMargin;
        }
    }
}
