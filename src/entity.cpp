#include "entity.h"
#include "sav.h"
#include "helpers.h"
#include "defines.h"
#include "draw.h"
#include "navigation.h"
#include "vision.h"

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, SavTextureAtlas *atlas, Level *level)
{
    EntityStore s = {};
    s.entityMax = entityMax;
    s.entities = MemoryArenaPushArrayAndZero(arena, entityMax, Entity);

    s.controlledEntityVisibleTiles = MemoryArenaPushArray(arena, level->w * level->h, u8);

    s.arena = arena;
    s.atlas = atlas;
    s.tilePxW = level->levelTilemap.tilePxW;
    s.tilePxH = level->levelTilemap.tilePxH;

    return s;
}

api_func Entity MakeEntity(f32 x, f32 y, Level *level, int atlasValue, v4 color, CountedString name)
{
    Entity e = {};
    Assert(name.size < ENTITY_NAME_CHARS);
    Strcpy(e.name, name.string);
    e.p = V2(x, y);
    e.level = level;
    e.atlasValue = atlasValue;
    e.color = color;
    e.isUsed = true;
    return e;
}

api_func void ConfigureActorEntity(Entity *e, ActorStats stats)
{
    e->stats = stats;
    e->stats.isConfigured = true;
}

api_func Entity *AddEntity(EntityStore *s, Entity e)
{
    // TODO: Slot reuse
    Assert(s->entityCount < s->entityMax);
    e.id = s->entityCount;
    Entity *entityInStore = s->entities + s->entityCount++;
    *entityInStore = e;
    return entityInStore;
}

api_func Entity *GetEntityAt(EntityStore *s, v2 p)
{
    for (int i = 0; i < s->entityCount; i++)
    {
        Entity *e = s->entities + i;
        v2 eMin = e->p - V2(0.5f, 0.5f);
        v2 eMax = e->p + V2(0.5f, 0.5f);
        if (InBounds(eMin, eMax, p))
        {
            return e;
        }
    }

    return NULL;
}

internal_func void resetActorOrder(ActorOrder *order)
{
    order->type = ACTOR_ORDER_NONE;
    order->isCompleted = false;
}

internal_func void processActorAI(EntityStore *s, Entity *e, f32 delta)
{
    Assert(e->stats.isConfigured);

    switch (e->aiState)
    {
        case ACTOR_AI_INIT:
        {
            e->aiState = ACTOR_AI_IDLE;
        } break;
        
        case ACTOR_AI_IDLE:
        {
            Assert(e->currentOrder.type == ACTOR_ORDER_NONE || e->currentOrder.type == ACTOR_ORDER_MOVE_TO_TARGET);
            
            if (e->currentOrder.type == ACTOR_ORDER_NONE)
            {
                if (RandomChance(0.5f))
                {
                    e->currentOrder.type = ACTOR_ORDER_MOVE_TO_TARGET;
                    e->currentOrder.movementTarget = e->p + GetRandomVec(5.0f);
                }
            }
            else if (e->currentOrder.type == ACTOR_ORDER_MOVE_TO_TARGET && e->currentOrder.isCompleted)
            {
                resetActorOrder(&e->currentOrder);
            }
            
            v2i entityPos = GetTilePFromFloatP(e->p);
            v2i playerPos = GetTilePFromFloatP(s->controlledEntity->p);
            if (IsInLineOfSight(e->level, entityPos, playerPos, e->stats.viewRadius))
            {
                resetActorOrder(&e->currentOrder);

                // TODO: Not sure if the same state should both issue and handle order completion.
                // I.e. the setting of e->currentOrder, doesn't happen until ACTOR_AI_FOLLOW state.
                // For some reason I want it to do that, but I can't tell why.
                // So I will ignore that intuition for now...
                e->currentOrder.type = ACTOR_ORDER_FOLLOW_ENTITY;
                e->currentOrder.followedEntity = s->controlledEntity;

                e->aiState = ACTOR_AI_FOLLOW;
            }
        } break;
        
        case ACTOR_AI_FOLLOW:
        {
            Assert(e->currentOrder.type == ACTOR_ORDER_FOLLOW_ENTITY);
            if (IsInRange(e->p, e->currentOrder.followedEntity->p, e->stats.attackReach))
            {
                resetActorOrder(&e->currentOrder);

                // TODO: Issue combat order here, or in ACTOR_AI_COMBAT?
                
                e->aiState = ACTOR_AI_COMBAT;
            }

            // TODO: If lost target (let's say invisible), return to IDLE.
            // But should still go to last know location.
        } break;

        case ACTOR_AI_COMBAT:
        {
            // TODO: Implement
        } break;
        
        default: InvalidCodePath; // Unhandled Actor AI state
    }
}

internal_func inline f32 getRotationSpeed(Entity *e)
{
    // TODO: Revisit this. Idk if it even makes sense to scale rotation speed with stats
    return e->stats.speed / ENTITY_BASE_SPEED * ENTITY_BASE_ROTATION_SPEED;
}

internal_func b32 steerEntity(Entity *e, v2 target, f32 delta)
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

internal_func b32 navigateToTarget(Entity *e, v2 target, f32 dT, MemoryArena *arena)
{
    if (!e->steerTargetActive)
    {
        MemoryArenaFreeze(arena);
        NavPath path = NavPathToTarget(e->level, e->p, target, arena);
        if (path.found && !path.alreadyAtTarget)
        {
            e->steerTarget = path.path[0];
            e->steerTargetActive = true;
        }
        else
        {
            // TODO: If path not found, communicate that somehow?
            MemoryArenaUnfreeze(arena);
            return true;
        }
        MemoryArenaUnfreeze(arena);
    }
    
    if (e->steerTargetActive && steerEntity(e, e->steerTarget, dT))
    {
        e->steerTargetActive = false;
        f32 dist = VecLength(e->p - target);
        // TODO: dist <= e->stats.moveSpeed * dT -- not sure if that part is right.
        // It would be better to see that our steerTarget was the same as movement target and not re do the distance check
        if (dist <= e->stats.speed * dT || dist < (f32)CMP_EPSILON)
        {
            return true;
        }
    }

    return false;
}

internal_func void processActorOrders(EntityStore *s, Entity *e, f32 dT)
{
    Assert(e->stats.isConfigured);

    switch(e->currentOrder.type)
    {
        case ACTOR_ORDER_NONE:
        {
            // Do nothing
        } break;
        
        case ACTOR_ORDER_MOVE_TO_TARGET:
        {
            if (!e->currentOrder.isCompleted && navigateToTarget(e, e->currentOrder.movementTarget, dT, s->arena))
            {
                e->currentOrder.isCompleted = true;
            }
        } break;
        
        case ACTOR_ORDER_FOLLOW_ENTITY:
        {
            // TODO: Implement
        } break;

        default: InvalidCodePath; // Unhandled actor order type
    }

    // else if (e->brain.isOrderedFollow)
    // {
    //     moveEntity(e, e->brain.followedEntity->p, delta);
    //     if (IsInRange(e->p, e->brain.followedEntity->p, 2.25f))
    //     {
    //         // TODO: Is this right?
    //         // "Follow" means keep following, if they move away.
    //         // But that's handled by ai state reverting back to idle and then following again.
    //         e->brain.isOrderedFollow = false;
    //     }
    // }
    // else if (e->brain.isOrderedMovement)
    // {
    //     // TraceLog("Moving towards: %.3f, %.3f", e->brain.pathNextTarget.x, e->brain.pathNextTarget.y);
    // }
}

api_func void UpdateEntities(EntityStore *s, f32 delta)
{
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        if (!IsControlledEntity(s, e))
        {
            processActorAI(s, e, delta);
        }

        processActorOrders(s, e, delta);
    }

    Entity *controlled = s->controlledEntity;
    v2i tileP = GetTilePFromFloatP(controlled->p);
    CalculateVision(controlled->level, tileP, controlled->stats.viewRadius, s->controlledEntityVisibleTiles);
}

api_func void DrawEntities(EntityStore *s)
{
    int vertCount = s->entityCount * 4;
    int indexCount = s->entityCount * 6;

    MemoryArenaFreeze(s->arena);

    v3 *positions = MemoryArenaPushArrayAndZero(s->arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
    v4 *vertColors = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
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

        u32 indexBase = vertsAdded;
        for (int i = 0; i < 4; i++)
        {
            positions[vertsAdded] = points.e[i];
            texCoords[vertsAdded] = texCoordPoints.e[i];
            vertColors[vertsAdded] = e->color;
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
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(vertColors, vertCount, sizeof(vertColors[0])));
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

        if (e->currentOrder.type == ACTOR_ORDER_MOVE_TO_TARGET)
        {
            v2 targetPxP = V2(e->currentOrder.movementTarget.x * s->tilePxW, e->currentOrder.movementTarget.y * s->tilePxH);
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
