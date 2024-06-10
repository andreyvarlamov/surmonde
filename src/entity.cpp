#include "entity.h"
#include "sav.h"
#include "helpers.h"

api_func EntityStore MakeEntityStore(int entityMax, MemoryArena *arena, SavTextureAtlas *atlas, f32 tilePxW, f32 tilePxH)
{
    EntityStore s = {};
    s.entityMax = entityMax;
    s.entities = MemoryArenaPushArrayAndZero(arena, entityMax, Entity);

    // s.w = spatialW;
    // s.h = spatialH;
    // s.spatialEntities = MemoryArenaPushArrayAndZero(arena, spatialW * spatialH, Entity *);

    s.arena = arena;
    s.atlas = atlas;
    s.tilePxW = tilePxW;
    s.tilePxH = tilePxH;

    return s;
}

#if 0
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
#endif

api_func Entity MakeEntity(f32 x, f32 y, Level *level, i32 atlasValue, SavColor bg, SavColor fg, f32 speed)
{
    Entity e;
    e.p = V2(x, y);
    e.level = level;
    e.atlasValue = atlasValue;
    e.bg = bg;
    e.fg = fg;
    e.speed = speed;
    e.brain = {};
    e.isUsed = true;
    return e;
}

api_func Entity *AddEntity(EntityStore *s, Entity e)
{
    // TODO: Slot reuse
    Assert(s->entityCount < s->entityMax);
    Entity *entityInStore = s->entities + s->entityCount++;
    *entityInStore = e;
    // addSpatialEntity(s, entityInStore);
    return entityInStore;
}

api_func void MoveEntity(EntityStore *s, Entity *e, v2 dp)
{
    // removeSpatialEntity(s, e);
    e->p += dp;
    // addSpatialEntity(s, e);
}

api_func void OrderEntityMovement(Entity *e, v2 target)
{
    e->brain.isOrderedMovement = true;
    // TODO: Check if target is valid
    e->brain.movementTarget = target;
}

api_func void UpdateEntities(EntityStore *s, f32 delta)
{
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        if (e != s->controlledEntity && !e->brain.isOrderedMovement)
        {
            if (GetRandomValue(0, 100) == 0)
            {
                e->brain.isOrderedMovement = true;
                e->brain.movementTarget = e->p + GetRandomVec(2.0f + GetRandomFloat() * 5.0f);
            }
        }

        if (e->brain.isOrderedMovement)
        {
            v2 newP = VecNormalize(e->brain.movementTarget - e->p) * e->speed * delta;
            // TODO: Pathfind to target
            MoveEntity(s, e, newP);
            if (VecEqual(e->p, e->brain.movementTarget, 0.1f))
            {
                e->brain.isOrderedMovement = false;
            }
        }
    }
}

api_func void DrawEntities(EntityStore *s)
{
    int vertCount = s->entityCount * 4;
    int indexCount = s->entityCount * 6;

    MemoryArenaFreeze(s->arena);

    v3 *positions = MemoryArenaPushArrayAndZero(s->arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
    v4 *colorsBg = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
    v4 *colorsFg = MemoryArenaPushArrayAndZero(s->arena, vertCount, v4);
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
            colorsBg[vertsAdded] = GetColorV4(e->bg);
            colorsFg[vertsAdded] = GetColorV4(e->fg);
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
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colorsBg, vertCount, sizeof(colorsBg[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords, vertCount, sizeof(texCoords[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colorsFg, vertCount, sizeof(colorsFg[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    BindTextureSlot(0, s->atlas->texture);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    UnbindTextureSlot(0);

    MemoryArenaUnfreeze(s->arena);
}
