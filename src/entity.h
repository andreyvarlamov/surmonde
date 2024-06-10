#ifndef ENTITY_H
#define ENTITY_H

#include "sav.h"
#include "tilemap.h"

struct Level;

struct Entity
{
    v2i p;
    Level *level;
    Tile tile;
    b32 isUsed;
    i64 energy;
};

#define MAX_CONTROLLED_ENTITIES 16
struct EntityStore
{
    int entityCount;
    int entityMax;
    Entity *entities;

    int w, h;
    Entity **spatialEntities;

    int controlledEntityCount;
    Entity *controlledEntities[MAX_CONTROLLED_ENTITIES];

    MemoryArena *arena;
};

struct PlayerInput
{
    int a;
};

api_func EntityStore MakeEntityStore(MemoryArena *arena, int entityMax, int spatialW, int spatialH);
api_func Entity MakeEntity(int x, int y, Level *level, Tile tile);
api_func void AddEntity(EntityStore *entityStore, Entity entity);
api_func void MoveEntity(EntityStore *entityStore, Entity *entity, v2i p);
api_func void PopulateTilemap(EntityStore *entityStore, Tilemap *tilemap);
api_func i64 ProcessEntityTurn(EntityStore *s, Entity *e);
api_func i64 ProcessControlledEntity(EntityStore *s, PlayerInput input, Entity *e);
api_func b32 IsControlledEntity(EntityStore *s, Entity *e);

#endif
