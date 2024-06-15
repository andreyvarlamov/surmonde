#include "entity.h"
#include "sav.h"
#include "helpers.h"
#include "settings.h"

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

#if 0
internal_func void addSpatialEntity(EntityStore *s, Entity *e)
{
    int i = XYToIdx(s->w, e->p.x, e->p.y);
    // TODO: Multiple entities per spatial slot
    Assert(s->spatialEntities[i] == NULL);
    s->spatialEntities[i] = e;
}

internal_func void removeSpatialEntity(EntityStore *s, Entity *e)
{
    int i = XYToIdx(s->w, e->p.x, e->p.y);
    Assert(s->spatialEntities[i] == e);
    s->spatialEntities[i] = NULL;
}
#endif

api_func Entity MakeEntity(f32 x, f32 y, Level *level, i32 atlasValue, SavColor bg, SavColor fg)
{
    Entity e;
    e.p = V2(x, y);
    e.level = level;
    e.atlasValue = atlasValue;
    e.bg = bg;
    e.fg = fg;
    e.brain = {};
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

api_func void OrderEntityMovement(Entity *e, v2 target)
{
    e->brain.isOrderedMovement = true;
    // TODO: Check if target is valid
    e->brain.movementTarget = target;
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

    defender->brain.combatState = COMBAT_STATE_READY;
    defender->brain.opponent = attacker;
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
    if (AbsF32(angle - e->yawDeg) > QUICK_TURN_AROUND_MAX)
    {    
       	moveAmount = e->stats.speed * 0.2f * delta;
    }
    else
    {
        moveAmount = e->stats.speed * delta;
    }

    // TODO: Pathfind to target
    MoveTowardAngleDeg(&e->yawDeg, angle, getRotationSpeed(e) * delta);
    b32 targetReached = MoveToward(&e->p, target, moveAmount);

    return targetReached;
}

internal_func inline void setEntityCombatState(Entity *e, CombatState state, f32 howLong)
{
    e->brain.combatState = state;
    e->brain.combatTimer = howLong;
    TraceLog("%p combat state: %d for %.1f s", state, howLong);
}

internal_func inline b32 performAttackHit(Entity *attacker, Entity *defender)
{
    b32 success = defender->brain.combatState != COMBAT_STATE_DEFENCE_READY;
    return success;
}

internal_func void processCharacterAI(EntityStore *s, Entity *e)
{
    Assert(e->stats.isConfigured);

   if (e->brain.combatState == COMBAT_STATE_NONE && isEntityInRange(e, s->controlledEntity, e->stats.attackReach))
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
            OrderEntityMovement(e, e->p + GetRandomVec(2.0f + GetRandomFloat() * 5.0f));
        }
    }
}

internal_func void processCharacterOrders(EntityStore *s, Entity *e, f32 delta)
{
    Assert(e->stats.isConfigured);

    if (e->brain.combatState != COMBAT_STATE_NONE)
    {
        switch (e->brain.combatState)
        {
            case COMBAT_STATE_READY:
            {
                if (e->brain.combatInitiative > e->brain.opponent->brain.combatInitiative)
                {
                    setEntityCombatState(e, COMBAT_STATE_ATTACK_PRE, 1.0f);
                }
                
                if (e->brain.opponent->brain.combatState == COMBAT_STATE_ATTACK_PRE)
                {
                    setEntityCombatState(e, COMBAT_STATE_DEFENCE_PRE, 0.5f);
                }
            } break;
            case COMBAT_STATE_ATTACK_PRE:
            {
                if (AdvanceTimer(&e->brain.combatTimer, delta))
                {
                    b32 attackSuccess = performAttackHit(e, e->brain.opponent);
                    TraceLog("%p hits %p: %d", e, e->brain.opponent, attackSuccess);
                    setEntityCombatState(e, COMBAT_STATE_ATTACK_POST, 1.0f);
                }
                
            } break;

            case COMBAT_STATE_ATTACK_POST:
            {
                if (AdvanceTimer(&e->brain.combatTimer, delta))
                {
                    setEntityCombatState(e, COMBAT_STATE_READY, 0.0f);
                }
            } break;
            
            case COMBAT_STATE_DEFENCE_PRE:
            {
                if (AdvanceTimer(&e->brain.combatTimer, delta))
                {
                    setEntityCombatState(e, COMBAT_STATE_DEFENCE_READY, 0.0f);
                }
            } break;

            case COMBAT_STATE_DEFENCE_READY:
            {
                if (e->brain.opponent->brain.combatState == COMBAT_STATE_ATTACK_POST)
                {
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

            default: break;
        }
    }
    else if (e->brain.isOrderedFollow)
    {
        moveEntity(e, e->brain.followedEntity->p, delta);
    }
    else if (e->brain.isOrderedMovement)
    {
        if (moveEntity(e, e->brain.movementTarget, delta))
        {
            e->brain.isOrderedMovement = false;
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
        DrawFilledCircleSegment(pxP, 64.0f, e->yawDeg - 30.0f, e->yawDeg + 30.0f, ColorAlpha(SAV_COLOR_YELLOW, 0.1f), s->arena);

        if (e->brain.isOrderedMovement)
        {
            v2 targetPxP = V2(e->brain.movementTarget.x * s->tilePxW, e->brain.movementTarget.y * s->tilePxH);
            DrawLine(pxP, targetPxP, SAV_COLOR_DARKSLATEGRAY);
        }
    }
#endif
}
