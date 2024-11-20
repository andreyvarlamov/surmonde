#ifndef ENTITY_H
#define ENTITY_H

#include "sav.h"
#include "helpers.h"
#include "navigation.h"
#include "defines.h"
#include "inventory.h"

struct LevelStore;
struct Level;
struct Entity;

enum ActorAiStateType
{
    ACTOR_AI_INIT,
    ACTOR_AI_IDLE,
    ACTOR_AI_MOVE_TO_TARGET,
    ACTOR_AI_FOLLOW_ENTITY,
    ACTOR_AI_COMBAT
};
global_var char *ActorAiStateTypeString[] = {"Init", "Idle", "Move To Target", "Follow Entity", "Combat"};

struct ActorAiState
{
    ActorAiStateType type;

    b32 hasRandomTarget;
    v2 movementTarget;
    Entity *entityToFollow;
    Entity *entityToAttack;
};

enum ActorOrderType
{
    ACTOR_ORDER_NONE,
    ACTOR_ORDER_MOVE_TO_TARGET,
    ACTOR_ORDER_FOLLOW_ENTITY,
    ACTOR_ORDER_ATTACK_ENTITY,
};
global_var char *ActorOrderTypeString[] = {"None", "Move To Target", "Follow Entity", "Attack Entity"};

struct ActorOrder
{
    ActorOrderType type;

    v2 movementTarget;
    Entity *entityToFollow;
    Entity *entityToAttack;
    f32 attackTimer;

    b32 isCompleted;
};

struct ActorStats
{
    b32 isConfigured;
    
    i32 viewRadius;
    f32 attackReach;
    f32 speed;
    f32 combatRadius;

    f32 health;
    f32 maxHealth;
};

enum EntityType
{
    EntityType_None,
    EntityType_Actor,
    EntityType_Container,
    EntityType_ItemPickup,
    EntityType_Machine
};
global_var char *EntityTypeString[] = {"None", "Actor", "Container", "Item Pickup", "Machine"};

#include "entity_machine.h"

struct Entity
{
    i32 id;

    EntityType type;
    
    v2 p;
    f32 yawDeg;

    Level *level;

    Sprite sprite;
    v4 color;

    char name[ENTITY_NAME_CHARS];

    ActorStats stats;

    ActorAiState aiState;
    ActorOrder currentOrder; // TODO: Order queue

    b32 steerTargetActive;
    v2 steerTarget;

    InventoryBlock *inventory;

    b32 itemPickupNeedsUpdate;

    EntityDataMachine machineData;

    b32 isPaused;
    b32 isBlocking;
    b32 isOpaque;
};

struct EntityStore
{
    int entityCount;
    int entityMax;
    Entity *entities;

    Entity *controlledEntity;
    u8 *controlledEntityVisibleTiles;

    MemoryArena *arena;

    // TODO: This shouldn't be here
    f32 tilePxW;
    f32 tilePxH;
};

inline b32 IsControlledEntity(EntityStore *s, Entity *e)
{
    return e == s->controlledEntity;
}

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, f32 tilePxW, f32 tilePxH, int levelTileCount);
api_func Entity MakeEntity(EntityType type, f32 x, f32 y, Level *level, Sprite sprite, v4 color, CountedString name, b32 isBlocking, b32 isOpaque);
api_func void ConfigureActorEntity(Entity *e, ActorStats stats);
api_func Entity *AddEntity(EntityStore *s, Entity e);

struct EntitySearchResult
{
    Entity *entities[ENTITY_SEARCH_RESULT_MAX];
    int entityCount;
};
api_func EntitySearchResult GetEntityAt(EntityStore *s, v2 p);
api_func EntitySearchResult GetEntityOfTypeAt(EntityStore *s, EntityType type, v2 p);

api_func Entity *AddActorEntity(EntityStore *s, f32 x, f32 y, Level *level, Sprite sprite, CountedString name);
api_func Entity *AddItemPickupEntity(EntityStore *s, f32 x, f32 y, Level *level);
 
api_func b32 AnyBlockingEntityAtTile(EntityStore *s, v2i tile);
api_func b32 AnyOpaqueEntityAtTile(EntityStore *s, v2i tile);
api_func void ResetActorAI(Entity *e);

api_func void AddItemToEntityInventory(Entity *e, InventoryStore *inventoryStore, InventoryItem item);
api_func void RemoveItemFromEntityInventory(Entity *e, InventoryStore *inventoryStore, InventoryItem *item);
api_func void MoveItemFromEntityToEntity(Entity *sourceEntity, Entity *destEntity, InventoryStore *inventoryStore, InventoryItem *item);
api_func void DropItemFromEntity(EntityStore *s, Entity *e, InventoryStore *inventoryStore, InventoryItem *item);
    
api_func b32 OrderEntityMoveToTarget(Entity *e, v2 movementTarget);

api_func void MoveEntity(EntityStore *s, Entity *e, v2 dp);
api_func void UpdateEntities(EntityStore *s, f32 dT, InventoryStore *inventoryStore, LevelStore *levelStore);

api_func void DrawEntities(EntityStore *s, LevelStore *levelStore);

#endif
