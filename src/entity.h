#ifndef ENTITY_H
#define ENTITY_H

#include "sav.h"

struct Level;

struct EntityBrain
{
    b32 isOrderedMovement;
    v2 movementTarget;
};

struct Entity
{
    v2 p;
    Level *level;

    i32 atlasValue;
    SavColor fg;
    SavColor bg;

    f32 speed;
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

struct EntityIterator
{
    Entity *e;
    int i;
};

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, SavTextureAtlas *atlas, f32 tilePxW, f32 tilePxH);
api_func Entity MakeEntity(f32 x, f32 y, Level *level, i32 atlasValue, SavColor bg, SavColor fg, f32 speed);
api_func Entity *AddEntity(EntityStore *s, Entity e);

api_func void MoveEntity(EntityStore *s, Entity *e, v2 dp);
api_func void OrderEntityMovement(Entity *e, v2 target);
api_func void UpdateEntities(EntityStore *s, f32 delta);

api_func void DrawEntities(EntityStore *s);

#endif
