#include "entity.h"
#include "sav.h"
#include "helpers.h"
#include "settings.h"

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

api_func void OrderEntityMovement(Entity *e, v2 target)
{
    e->brain.isOrderedMovement = true;
    // TODO: Check if target is valid
    e->brain.movementTarget = target;
}

internal_func inline f32 getRotationSpeed(Entity *e)
{
    // TODO: Revisit this. Idk if it even makes sense to scale rotation speed with stats
    return e->speed / ENTITY_BASE_SPEED * ENTITY_BASE_ROTATION_SPEED;
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
                OrderEntityMovement(e, e->p + GetRandomVec(2.0f + GetRandomFloat() * 5.0f));
            }
        }

        if (e->brain.isOrderedMovement)
        {
            v2 movement = e->brain.movementTarget - e->p;
            f32 dist = VecLength(movement);
            if (dist < CMP_EPSILON)
            {
                e->brain.isOrderedMovement = false;
                continue;
            }

            v2 dir = movement / dist;
            f32 angle = ToDeg(GetVecFlippedYAngle(dir));

            f32 moveAmount;
            if (AbsF32(angle - e->yawDeg) > QUICK_TURN_AROUND_MAX)
            {
                moveAmount = e->speed * 0.2f * delta;
            }
            else
            {
                moveAmount = e->speed * delta;
            }

            // TODO: Pathfind to target
            MoveTowardAngleDeg(&e->yawDeg, angle, getRotationSpeed(e) * delta);
            if (MoveToward(&e->p, e->brain.movementTarget, moveAmount))
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

#if (DRAW_ENTITY_DEBUG == 1)
    for (int entityIndex = 0; entityIndex < s->entityCount; entityIndex++)
    {
        Entity *e = s->entities + entityIndex;

        v2 pxP = V2(e->p.x * s->tilePxW, e->p.y * s->tilePxH);
        DrawFilledCircleSegment(pxP, 64.0f, e->yawDeg - 30.0f, e->yawDeg + 30.0f, ColorAlpha(SAV_COLOR_YELLOW, 0.1f), s->arena);

        if (e->brain.isOrderedMovement)
        {
            v2 targetPxP = V2(e->brain.movementTarget.x * s->tilePxW, e->brain.movementTarget.y * s->tilePxH);
            DrawLine(pxP, targetPxP, SAV_COLOR_DARKSLATEGRAY);
        }
    }
#endif
}
