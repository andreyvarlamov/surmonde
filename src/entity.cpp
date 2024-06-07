#include "entity.h"
#include "sav.h"
#include "tilemap.h"
#include "helpers.h"

api_func EntityStore MakeEntityStore(MemoryArena *arena, int entityMax, int spatialW, int spatialH)
{
    EntityStore entityStore;
    entityStore.entityCount = 0;
    entityStore.entityMax = entityMax;
    entityStore.entities = MemoryArenaPushArrayAndZero(arena, entityMax, Entity);

    entityStore.w = spatialW;
    entityStore.h = spatialH;
    entityStore.spatialEntities = MemoryArenaPushArrayAndZero(arena, spatialW * spatialH, Entity *);
    return entityStore;
}

internal_func void addSpatialEntity(EntityStore *entityStore, Entity *entity)
{
    int i = XYToIdx(entityStore->w, entity->p.x, entity->p.y);
    // TODO: Multiple entities per spatial slot
    Assert(entityStore->spatialEntities[i] == NULL);
    entityStore->spatialEntities[i] = entity;
}

internal_func void removeSpatialEntity(EntityStore *entityStore, Entity *entity)
{
    int i = XYToIdx(entityStore->w, entity->p.x, entity->p.y);
    Assert(entityStore->spatialEntities[i] == entity);
    entityStore->spatialEntities[i] = NULL;
}

api_func Entity MakeEntity(int x, int y, Level *level, Tile tile)
{
    Entity e;
    e.p = V2I(x, y);
    e.level = level;
    e.tile = tile;
    e.isUsed = true;
    return e;
}

api_func void AddEntity(EntityStore *entityStore, Entity entity)
{
    // TODO: Slot reuse
    Assert(entityStore->entityCount < entityStore->entityMax);
    Entity *entityInStore = entityStore->entities + entityStore->entityCount++;
    *entityInStore = entity;
    addSpatialEntity(entityStore, entityInStore);
}

api_func void MoveEntity(EntityStore *entityStore, Entity *entity, v2i p)
{
    removeSpatialEntity(entityStore, entity);
    entity->p = p;
    addSpatialEntity(entityStore, entity);
}

api_func void PopulateTilemap(EntityStore *entityStore, Tilemap *tilemap)
{
    for (int i = 0; i < entityStore->entityCount; i++)
    {
        Entity *entity = entityStore->entities + i;
        if (entity->isUsed)
        {
            SetTile(tilemap, entity->p.x, entity->p.y, entity->tile);
        }
    }
}