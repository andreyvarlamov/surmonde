#include "entity.h"
#include "sav.h"
#include "helpers.h"
#include "defines.h"
#include "draw.h"
#include "navigation.h"
#include "vision.h"
#include "ui_game_log.h"
#include "level.h"

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, f32 tilePxW, f32 tilePxH, int levelTileCount)
{
    EntityStore s = {};
    s.entityMax = entityMax;
    s.entities = MemoryArenaPushArrayAndZero(arena, entityMax, Entity);

    s.controlledEntityVisibleTiles = MemoryArenaPushArray(arena, levelTileCount, u8);

    s.arena = arena;
    s.tilePxW = tilePxW;
    s.tilePxH = tilePxH;

    return s;
}

api_func Entity MakeEntity(EntityType type, f32 x, f32 y, Level *level, Sprite sprite, v4 color, CountedString name, b32 isBlocking, b32 isOpaque)
{
    Entity e = {};
    
    Assert(name.size < ENTITY_NAME_CHARS);
    Strcpy(e.name, name.string);

    e.type = type;

    e.p = V2(x, y);
    e.level = level;
    e.sprite = sprite;
    e.color = color;
    e.isBlocking = isBlocking;
    e.isOpaque = isOpaque;

    return e;
}

api_func void ConfigureActorEntity(Entity *e, ActorStats stats)
{
    e->stats = stats;
    e->stats.isConfigured = true;
}

api_func Entity *AddEntity(EntityStore *s, Entity e)
{
    Assert(s->entityCount < s->entityMax);
    e.id = s->entityCount;
    Entity *entityInStore = s->entities + s->entityCount++;
    *entityInStore = e;
    return entityInStore;
}

api_func EntitySearchResult GetEntityAt(EntityStore *s, v2 p)
{
    EntitySearchResult result = {};
    for (int i = 0; i < s->entityCount; i++)
    {
        Entity *e = s->entities + i;
        if (e->type != EntityType_None)
        {
            v2 eMin = e->p - V2(0.5f, 0.5f);
            v2 eMax = e->p + V2(0.5f, 0.5f);
            if (InBounds(eMin, eMax, p))
            {
                Assert(result.entityCount < ENTITY_SEARCH_RESULT_MAX);
                result.entities[result.entityCount++] = e;
            }
        }
    }
    return result;
}

api_func EntitySearchResult GetEntityOfTypeAt(EntityStore *s, EntityType type, v2 p)
{
    EntitySearchResult result = {};
    for (int i = 0; i < s->entityCount; i++)
    {
        Entity *e = s->entities + i;
        if ((type == EntityType_None && e->type != EntityType_None) || e->type == type)
        {
            v2 eMin = e->p - V2(0.5f, 0.5f);
            v2 eMax = e->p + V2(0.5f, 0.5f);
            if (InBounds(eMin, eMax, p))
            {
                Assert(result.entityCount < ENTITY_SEARCH_RESULT_MAX);
                result.entities[result.entityCount++] = e;
            }
        }
    }
    return result;
}

api_func Entity *AddActorEntity(EntityStore *s, f32 x, f32 y, Level *level, Sprite sprite, CountedString name)
{
    Assert(s->entityCount < s->entityMax);
    
    int id = s->entityCount;
    Entity *e = s->entities + s->entityCount++;

    *e = {};

    e->id = id;
    e->type = EntityType_Actor;

    Assert(name.size < ENTITY_NAME_CHARS);
    Strcpy(e->name, name.string);

    e->p = V2(x, y);
    e->level = level;
    e->sprite = sprite;
    e->color = V4(1.0f, 1.0f, 1.0f, 1.0f);
    e->isBlocking = true;
    e->isOpaque = false;

    return e;
}

api_func Entity *AddItemPickupEntity(EntityStore *s, f32 x, f32 y, Level *level)
{
    Assert(s->entityCount < s->entityMax);
    int id = s->entityCount;
    Entity *e = s->entities + s->entityCount++;

    *e = {};

    e->id = id;
    e->type = EntityType_ItemPickup;

    Strcpy(e->name, "Item pickup");

    e->p = V2(x, y);
    e->level = level;
    e->color = V4(1.0f, 1.0f, 1.0f, 1.0f);
    
    return e;
}

api_func b32 AnyBlockingEntityAtTile(EntityStore *s, v2i tile)
{
#if 1
    b32 result = false;
    for (int i = 0; i < s->entityCount; i++)
    {
        Entity *e = s->entities + i;
        if (e->type != EntityType_None && e->isBlocking && (i32)e->p.x == tile.x && (i32)e->p.y == tile.y)
        {
            result = true;
            break;
        }
    }
    return result;
#else
    return false;
#endif
}

api_func b32 AnyOpaqueEntityAtTile(EntityStore *s, v2i tile)
{
#if 1
    b32 result = false;
    for (int i = 0; i < s->entityCount; i++)
    {
        Entity *e = s->entities + i;
        if (e->type != EntityType_None && e->isOpaque && (i32)e->p.x == tile.x && (i32)e->p.y == tile.y)
        {
            result = true;
            break;
        }
    }
    return result;
#else
    return false;
#endif
}

internal_func void refreshItemPickup(Entity *e, InventoryStore *inventoryStore)
{
    InventoryItemIterator it = GetInventoryItemIterator(inventoryStore, e->inventory);
    InventoryItem *firstItem = NextInventoryItem(&it);
    if (firstItem)
    {
        e->sprite = firstItem->spec->sprite;
    }
    else
    {
        e->type = EntityType_None;
    }
}

api_func void AddItemToEntityInventory(Entity *e, InventoryStore *inventoryStore, InventoryItem item)
{
    AddItemToInventory(inventoryStore, &e->inventory, item, e);
    if (e->type == EntityType_ItemPickup)
    {
        refreshItemPickup(e, inventoryStore);
    }
}

api_func void RemoveItemFromEntityInventory(Entity *e, InventoryStore *inventoryStore, InventoryItem *item)
{
    RemoveItemFromInventory(inventoryStore, &e->inventory, item);
    if (e->type == EntityType_ItemPickup)
    {
        refreshItemPickup(e, inventoryStore);
    }
}

api_func void MoveItemFromEntityToEntity(Entity *sourceEntity, Entity *destEntity, InventoryStore *inventoryStore, InventoryItem *item)
{
    AddItemToEntityInventory(destEntity, inventoryStore, *item);
    RemoveItemFromEntityInventory(sourceEntity, inventoryStore, item);
}

api_func void DropItemFromEntity(EntityStore *s, Entity *e, InventoryStore *inventoryStore, InventoryItem *item)
{
    Entity *itemPickup = GetEntityOfTypeAt(s, EntityType_ItemPickup, e->p).entities[0];
    if (itemPickup == NULL)
    {
        itemPickup = AddItemPickupEntity(s, e->p.x, e->p.y, e->level);
    }
    AddItemToEntityInventory(itemPickup, inventoryStore, *item);
    RemoveItemFromEntityInventory(e, inventoryStore, item);
}

api_func void ResetActorAI(Entity *e)
{
    e->aiState.type = ACTOR_AI_INIT;
}

internal_func void entityDie(Entity *e)
{
    AddGameLogEntry("%s dies.", e->name);
    e->type = EntityType_None;
}

internal_func f32 entitySetHealth(EntityStore *s, Entity *e, f32 health)
{
    f32 delta = 0.0f;
    b32 godMode = PLAYER_GOD_MODE;
    if (!godMode || !IsControlledEntity(s, e) || delta > 0.0f)
    {
        f32 oldHealth = e->stats.health;
        e->stats.health = health;
        if (e->stats.health < 0.1f)
        {
            e->stats.health = 0.0f;
            health = 0.0f;
            entityDie(e);
        }
        delta = health - oldHealth;
    }
    return delta;
}

internal_func void resetActorOrder(Entity *e)
{
    e->currentOrder.type = ACTOR_ORDER_NONE;
    e->currentOrder.isCompleted = false;
    e->steerTargetActive = false;
}

internal_func b32 orderMoveToTarget(Entity *e, v2 movementTarget)
{
    if (e->currentOrder.type == ACTOR_ORDER_MOVE_TO_TARGET && VecEqualExact(e->currentOrder.movementTarget, movementTarget))
    {
        b32 isCompleted = e->currentOrder.isCompleted;
        if (isCompleted)
        {
            resetActorOrder(e);
        }
        return isCompleted;
    }
    else
    {
        e->currentOrder.type = ACTOR_ORDER_MOVE_TO_TARGET;
        e->currentOrder.movementTarget = movementTarget;
        return false;
    }
}

internal_func b32 orderFollowEntity(Entity *e, Entity *entityToFollow)
{
    if (e->currentOrder.type == ACTOR_ORDER_FOLLOW_ENTITY && e->currentOrder.entityToFollow == entityToFollow)
    {
        b32 isCompleted = e->currentOrder.isCompleted;
        if (isCompleted)
        {
            resetActorOrder(e);
        }
        return isCompleted;
    }
    else
    {
        e->currentOrder.type = ACTOR_ORDER_FOLLOW_ENTITY;
        e->currentOrder.entityToFollow = entityToFollow;
        return false;
    }
}

internal_func b32 orderAttackEntity(Entity *e, Entity *entityToAttack)
{
    if (e->currentOrder.type == ACTOR_ORDER_ATTACK_ENTITY && e->currentOrder.entityToAttack == entityToAttack)
    {
        b32 isCompleted = e->currentOrder.isCompleted;
        if (isCompleted)
        {
            resetActorOrder(e);
        }
        return isCompleted;
    }
    else
    {
        e->currentOrder.type = ACTOR_ORDER_ATTACK_ENTITY;
        e->currentOrder.entityToAttack = entityToAttack;
        e->currentOrder.attackTimer = 0.0f;
        return false;
    }
}

internal_func b32 canEntitySeePosition(EntityStore *s, Entity *e, v2 p)
{
    b32 oldIsOpaque = e->isOpaque;
    e->isOpaque = false;
    b32 result = IsInLineOfSight(e->level, s, e->p, p, e->stats.viewRadius);
    e->isOpaque = oldIsOpaque;
    return result;
}

internal_func void processActorAI(EntityStore *s, Entity *e, f32 delta)
{
    Assert(e->stats.isConfigured);

    switch (e->aiState.type)
    {
        case ACTOR_AI_INIT:
        {
            resetActorOrder(e);
            e->aiState = {};
            e->aiState.type = ACTOR_AI_IDLE;
        } break;

        case ACTOR_AI_IDLE:
        {
            if (!IsControlledEntity(s, e))
            {
                if (!e->aiState.hasRandomTarget)
                {
                    if (RandomChance(0.5f))
                    {
                        e->aiState.hasRandomTarget = true;
                        e->aiState.movementTarget = e->p + GetRandomVec(5.0f);
                    }
                }
                else if (orderMoveToTarget(e, e->aiState.movementTarget))
                {
                    e->aiState.hasRandomTarget = false;
                }

                #if 0
                if (s->controlledEntity != NULL &&
                    s->controlledEntity->type == ENTITY_TYPE_ACTOR &&
                    canEntitySeePosition(s, e, s->controlledEntity->p))
                {
                    resetActorOrder(&e->currentOrder);

                    e->aiState.type = ACTOR_AI_FOLLOW_ENTITY;
                    e->aiState.entityToFollow = s->controlledEntity;
                }
                #endif
            }
        } break;

        case ACTOR_AI_MOVE_TO_TARGET:
        {
            if (orderMoveToTarget(e, e->aiState.movementTarget))
            {
                e->aiState.type = ACTOR_AI_IDLE;
            }
        } break;
        
        case ACTOR_AI_FOLLOW_ENTITY:
        {
            if (orderFollowEntity(e, e->aiState.entityToFollow))
            {
                orderFollowEntity(e, e->aiState.entityToFollow);
            }
            
            if (e->aiState.entityToFollow->type == EntityType_None ||!canEntitySeePosition(s, e, s->controlledEntity->p))
            {
                resetActorOrder(e);
                e->aiState.type = ACTOR_AI_IDLE;
            }
            else if (IsInRange(e->p, e->aiState.entityToFollow->p, e->stats.attackReach))
            {
                resetActorOrder(e);
                e->aiState.type = ACTOR_AI_COMBAT;
                e->aiState.entityToAttack = e->aiState.entityToFollow;
            }
        } break;

        case ACTOR_AI_COMBAT:
        {
            if (orderAttackEntity(e, e->aiState.entityToAttack))
            {
                if (e->aiState.entityToAttack->type == EntityType_None)
                {
                    resetActorOrder(e);
                    e->aiState.type = ACTOR_AI_IDLE;
                }
                else if (!IsInRange(e->p, e->aiState.entityToFollow->p, e->stats.attackReach))
                {
                    resetActorOrder(e);

                    if (canEntitySeePosition(s, e, s->controlledEntity->p))
                    {
                        e->aiState.type = ACTOR_AI_FOLLOW_ENTITY;
                        e->aiState.entityToFollow = e->aiState.entityToAttack;
                    }
                    else
                    {
                        e->aiState.type = ACTOR_AI_IDLE;
                    }
                }
                else
                {
                    orderAttackEntity(e, e->aiState.entityToAttack);
                }
            }

            
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

internal_func b32 navigateToTarget(EntityStore *s, Entity *e, v2 target, f32 dT, MemoryArena *arena)
{
    if (!e->steerTargetActive)
    {
        MemoryArenaFreeze(arena);
        
        b32 oldIsBlocking = e->isBlocking;
        e->isBlocking = false;

        NavPath path = NavPathToTarget(e->level, s, e->p, target, arena);

        e->isBlocking = oldIsBlocking;
        
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

internal_func b32 navigateToEntity(EntityStore *s, Entity *e, Entity *targetEntity, f32 dT, MemoryArena *arena)
{
    b32 oldTargetIsBlocking = targetEntity->isBlocking;
    targetEntity->isBlocking = false;
            
    b32 navigateDone = navigateToTarget(s, e, targetEntity->p, dT, s->arena);

    targetEntity->isBlocking = oldTargetIsBlocking;

    return navigateDone;
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
            if (!e->currentOrder.isCompleted && navigateToTarget(s, e, e->currentOrder.movementTarget, dT, s->arena))
            {
                e->currentOrder.isCompleted = true;
            }
        } break;
        
        case ACTOR_ORDER_FOLLOW_ENTITY:
        {
            if (!e->currentOrder.isCompleted && navigateToEntity(s, e, e->currentOrder.entityToFollow, dT, s->arena))
            {
                e->currentOrder.isCompleted = true;
            }
        } break;

        case ACTOR_ORDER_ATTACK_ENTITY:
        {
            if (!e->currentOrder.isCompleted)
            {
                if (e->currentOrder.attackTimer <= 0.0f && e->currentOrder.entityToAttack->type != EntityType_None)
                {
                    f32 delta = entitySetHealth(s, e->currentOrder.entityToAttack, e->currentOrder.entityToAttack->stats.health - 10.0f);
                    AddGameLogEntry("%s attacks %s for %.0f damage", e->name, e->currentOrder.entityToAttack->name, delta != 0.0f ? -delta : 0.0f);
                }
                e->currentOrder.attackTimer += dT;
                if (e->currentOrder.attackTimer >= 2.0f)
                {
                    e->currentOrder.isCompleted = true;
                }
            }
        } break;

        default: InvalidCodePath; // Unhandled actor order type
    }
}

#include "entity_machine.h"

api_func void UpdateEntities(EntityStore *s, f32 dT, InventoryStore *inventoryStore, LevelStore *levelStore, Camera2D *camera)
{
    Level *currentLevel = GetCurrentLevel(levelStore);
    
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        if (currentLevel != e->level)
        {
            continue;
        }

        switch (e->type)
        {
            case EntityType_None:
            {
            } break;

            case EntityType_Actor:
            {
                if (!e->isPaused)
                {
                    processActorAI(s, e, dT);
                    processActorOrders(s, e, dT);
                }
            } break;

            case EntityType_ItemPickup:
            {
            } break;

            case EntityType_Container:
            {
            } break;

            case EntityType_Machine:
            {
                UpdateMachineEntity(e, dT, inventoryStore);
            } break;

            default: InvalidCodePath; // Unhandled ENTITY_TYPE
        }
    }

    if (s->controlledEntity == NULL || s->controlledEntity->type == EntityType_None)
    {
        s->controlledEntity = NULL;
    }
    else
    {
        b32 oldIsOpaque = s->controlledEntity->isOpaque;
        s->controlledEntity->isOpaque = false;
        v2i tileP = GetTilePFromFloatP(s->controlledEntity->p);
        CalculateVision(s->controlledEntity->level, s, tileP, s->controlledEntity->stats.viewRadius, s->controlledEntityVisibleTiles);
        s->controlledEntity->isOpaque = oldIsOpaque;

        Direction4 whichDirApronIsPlayerOn = IsTileApron(s->controlledEntity->level, (int)s->controlledEntity->p.x, (int)s->controlledEntity->p.y);
        if (whichDirApronIsPlayerOn != Direction4_None)
        {
            Level *currentLevel = GetCurrentLevel(levelStore);
            v2i nextWorldPos = currentLevel->worldPos + Direction4_Vectors[whichDirApronIsPlayerOn];
            Level *nextLevel = GetLevelAtWorldPos(levelStore, s, nextWorldPos);
            SetCurrentLevel(levelStore, nextLevel);
            s->controlledEntity->level = nextLevel;

            switch (whichDirApronIsPlayerOn)
            {
                case Direction4_North:
                {
                    s->controlledEntity->p.y = nextLevel->h - 1.5f;
                } break;

                case Direction4_East:
                {
                    s->controlledEntity->p.x = 0.0f         + 1.5f;
                } break;

                case Direction4_South:
                {
                    s->controlledEntity->p.y = 0.0f         + 1.5f;
                } break;

                case Direction4_West:
                {
                    s->controlledEntity->p.x = nextLevel->w - 1.5f;
                } break;

                default: InvalidCodePath;
            }

            ResetActorAI(s->controlledEntity);

            v2 cameraTarget = V2(s->controlledEntity->p.x * s->tilePxW, s->controlledEntity->p.y * s->tilePxH);
            CameraSetTarget(camera, cameraTarget);
        }
    }
}

api_func void DrawEntities(EntityStore *s, LevelStore *levelStore)
{
    Level *currentLevel = GetCurrentLevel(levelStore);
    
    for (int currentAtlasI = SpriteAtlasName_None + 1; currentAtlasI < SpriteAtlasName_Count; currentAtlasI++)
    {
        SpriteAtlasName currentAtlasName = (SpriteAtlasName)currentAtlasI;
        SavTextureAtlas currentAtlas = GetAtlasByName(currentAtlasName);

        int entitiesToDraw = 0;
        for (int i = 0; i < s->entityCount; i++)
        {
            if (s->entities[i].type == EntityType_None ||
                s->entities[i].level != currentLevel ||
                s->entities[i].sprite.atlasName != currentAtlasName)
            {
                continue;
            }
            
            entitiesToDraw++;
        }
        
        if (entitiesToDraw == 0)
        {
            continue;
        }
    
        int vertCount = entitiesToDraw * 4;
        int indexCount = entitiesToDraw * 6;

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
            if (e->type == EntityType_None ||
                e->level != currentLevel ||
                e->sprite.atlasName != currentAtlasName)
            {
                continue;
            }

            f32 pxX = e->p.x * s->tilePxW - halfTilePxW;
            f32 pxY = e->p.y * s->tilePxH - halfTilePxH;
            Rect destRect = MakeRect(pxX, pxY, s->tilePxW, s->tilePxH);
            FourV3 points = ConvertFourV2V3(RectGetPoints(destRect));

            Rect atlasRect = GetAtlasSourceRect(currentAtlas, e->sprite.atlasValue);
            FourV4 texCoordPoints = ConvertFourV2V4(GetTextureRectTexCoords(currentAtlas.texture, atlasRect));

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

        BindTextureSlot(0, currentAtlas.texture);
        DrawVertexBatch(DEFAULT_VERTEX_BATCH);
        UnbindTextureSlot(0);

        MemoryArenaUnfreeze(s->arena);
    }

#if (DRAW_ENTITY_DEBUG == 1)
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;
        if (e->type == EntityType_None ||
            e->level != currentLevel)
        {
            continue;
        }

        if (!IsControlledEntity(s, e))
        {
            v2 pxP = V2(e->p.x * s->tilePxW, e->p.y * s->tilePxH);
            // NOTE: This visualization wouldn't work with non-square tiles.
            DrawFilledCircle(pxP, e->stats.viewRadius * s->tilePxW, ColorAlpha(SAV_COLOR_GRAY, 0.1f), s->arena);
        }

        v2 pxP = V2(e->p.x * s->tilePxW, e->p.y * s->tilePxH);
        // DrawFilledCircleSegment(pxP, 64.0f, e->yawDeg - 10.0f, e->yawDeg + 10.0f, ColorAlpha(SAV_COLOR_YELLOW, 0.1f), s->arena);

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
