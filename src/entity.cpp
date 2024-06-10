#include "entity.h"
#include "sav.h"
#include "tilemap.h"
#include "helpers.h"

api_func EntityStore MakeEntityStore(MemoryArena *arena, int entityMax, int spatialW, int spatialH)
{
    EntityStore s = {};
    s.entityMax = entityMax;
    s.entities = MemoryArenaPushArrayAndZero(arena, entityMax, Entity);

    s.w = spatialW;
    s.h = spatialH;
    s.spatialEntities = MemoryArenaPushArrayAndZero(arena, spatialW * spatialH, Entity *);

    s.arena = arena;

    return s;
}

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

api_func Entity MakeEntity(int x, int y, Level *level, Tile tile)
{
    Entity e;
    e.p = V2I(x, y);
    e.level = level;
    e.tile = tile;
    e.isUsed = true;
    return e;
}

api_func void AddEntity(EntityStore *s, Entity e)
{
    // TODO: Slot reuse
    Assert(s->entityCount < s->entityMax);
    Entity *entityInStore = s->entities + s->entityCount++;
    *entityInStore = e;
    addSpatialEntity(s, entityInStore);
}

api_func void MoveEntity(EntityStore *s, Entity *e, v2i p)
{
    removeSpatialEntity(s, e);
    e->p = p;
    addSpatialEntity(s, e);
}

api_func void PopulateTilemap(EntityStore *s, Tilemap *tilemap)
{
    for (int i = 0; i < s->entityCount; i++)
    {
        Entity *e = s->entities + i;
        if (e->isUsed)
        {
            SetTile(tilemap, e->p.x, e->p.y, e->tile);
        }
    }
}

api_func b32 IsControlledEntity(EntityStore *s, Entity *e)
{
    for (int i = 0; i < s->controlledEntityCount; i++)
    {
        if (s->controlledEntities[i] == e)
        {
            return true;
        }
    }
    return false;
}

api_func i64 ProcessEntityTurn(EntityStore *s, Entity *e)
{
    return 0;
}

api_func i64 ProcessControlledEntity(EntityStore *s, PlayerInput input, Entity *e)
{
    return 0;
}