#ifndef ENTITY_H
#define ENTITY_H

#include "sav.h"

struct Level;
struct Entity;

enum CombatState
{
    COMBAT_STATE_NONE,
    COMBAT_STATE_ATTACKING,
    COMBAT_STATE_DEFENDING
};

struct EntityBrain
{
    b32 isOrderedMovement;
    v2 movementTarget;

    b32 isOrderedFollow;
    Entity *followedEntity;

    CombatState combatState;
    Entity *opponent;
    f32 combatTimer;
};

struct CharacterStats
{
    b32 isConfigured;
    
    f32 viewRadius;
    f32 attackReach;
    f32 speed;
};

struct Entity
{
    v2 p;
    f32 yawDeg;

    Level *level;

    i32 atlasValue;
    SavColor fg;
    SavColor bg;

    CharacterStats stats;

    EntityBrain brain;

    b32 isUsed;
};

#define MAX_CONTROLLED_ENTITIES 16
struct EntityStore
{
    int entityCount;
    int entityMax;
    Entity *entities;

    // int w, h;
    // Entity **spatialEntities;

    Entity *controlledEntity;

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

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, SavTextureAtlas *atlas, f32 tilePxW, f32 tilePxH);
api_func Entity MakeEntity(f32 x, f32 y, Level *level, i32 atlasValue, SavColor bg, SavColor fg);
api_func void ConfigureCharacterEntity(Entity *e, CharacterStats stats);
api_func Entity *AddEntity(EntityStore *s, Entity e);

api_func void MoveEntity(EntityStore *s, Entity *e, v2 dp);
api_func void OrderEntityMovement(Entity *e, v2 target);
api_func void UpdateEntities(EntityStore *s, f32 delta);

api_func void DrawEntities(EntityStore *s);

#endif
