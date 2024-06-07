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
};

struct EntityStore
{
    int entityCount;
    int entityMax;
    Entity *entities;

    int w, h;
    Entity **spatialEntities;
};

api_func EntityStore MakeEntityStore(MemoryArena *arena, int entityMax, int spatialW, int spatialH);
api_func Entity MakeEntity(int x, int y, Level *level, Tile tile);
api_func void AddEntity(EntityStore *entityStore, Entity entity);
api_func void MoveEntity(EntityStore *entityStore, Entity *entity, v2i p);
api_func void PopulateTilemap(EntityStore *entityStore, Tilemap *tilemap);

#endif
