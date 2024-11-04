#ifndef ENTITY_H
#define ENTITY_H

#include "sav.h"
#include "helpers.h"
#include "navigation.h"
#include "defines.h"

struct Level;
struct Entity;

enum ActorAiState
{
    ACTOR_AI_INIT,
    ACTOR_AI_IDLE,
    ACTOR_AI_FOLLOW,
    ACTOR_AI_COMBAT
};
global_var char *ActorAiStateString[] = {"Init", "Idle", "Follow", "Combat"};

enum ActorOrderType
{
    ACTOR_ORDER_NONE,
    ACTOR_ORDER_MOVE_TO_TARGET,
    ACTOR_ORDER_FOLLOW_ENTITY,
};
global_var char *ActorOrderTypeString[] = {"None", "Move To Target", "Follow Entity"};

struct ActorOrder
{
    ActorOrderType type;

    v2 movementTarget;
    Entity *followedEntity;

    b32 isCompleted;
};

struct ActorStats
{
    b32 isConfigured;
    
    i32 viewRadius;
    f32 attackReach;
    f32 speed;
    f32 combatRadius;
};

struct Entity
{
    i64 id;
    
    v2 p;
    f32 yawDeg;

    Level *level;

    int atlasValue;
    v4 color;

    char name[ENTITY_NAME_CHARS];

    ActorStats stats;

    ActorAiState aiState;
    
    ActorOrder currentOrder; // TODO: Order queue

    b32 steerTargetActive;
    v2 steerTarget;

    b32 isUsed;
};

struct EntityStore
{
    int entityCount;
    int entityMax;
    Entity *entities;

    Entity *controlledEntity;
    u8 *controlledEntityVisibleTiles;

    MemoryArena *arena;

    SavTextureAtlas *atlas;
    // TODO: This shouldn't be here
    f32 tilePxW;
    f32 tilePxH;
};

inline b32 IsControlledEntity(EntityStore *s, Entity *e)
{
    return e == s->controlledEntity;
}

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, SavTextureAtlas *atlas, Level *level);
api_func Entity MakeEntity(f32 x, f32 y, Level *level, int atlasValue, v4 color, CountedString name);
api_func void ConfigureActorEntity(Entity *e, ActorStats stats);
api_func Entity *AddEntity(EntityStore *s, Entity e);
api_func Entity *GetEntityAt(EntityStore *s, v2 p);

api_func void MoveEntity(EntityStore *s, Entity *e, v2 dp);
api_func void UpdateEntities(EntityStore *s, f32 delta);

api_func void DrawEntities(EntityStore *s);

#endif
