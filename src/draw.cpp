#include "draw.h"
#include "sav.h"

api_func void DrawAtlasTilemap(
    SavTextureAtlas atlas,
    int tilemapWidth,
    int tilemapHeight,
    f32 tilePxW,
    f32 tilePxH,
    i32 *atlasValues,
    SavColor *colorsFg,
    SavColor *colorsBg,
    v2 origin,
    MemoryArena *arena)
{
    int tileCount = tilemapWidth * tilemapHeight;
    int vertCount = tileCount * 4;
    int indexCount = tileCount * 6;

    MemoryArenaFreeze(arena);

    v3 *positions = MemoryArenaPushArrayAndZero(arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    v4 *colFg = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    v4 *colBg = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(arena, indexCount, u32);
    int vertsAdded = 0;
    int indicesAdded = 0;
    for (int tileI = 0; tileI < tileCount; tileI++)
    {
        i32 tileVal = atlasValues[tileI];
        i32 tileX = tileI % tilemapWidth;
        i32 tileY = tileI / tilemapWidth;
        f32 pxX = ((f32) tileX + origin.x) * tilePxW;
        f32 pxY = ((f32) tileY + origin.y) * tilePxH;
        Rect destRect = MakeRect(pxX, pxY, tilePxW, tilePxH);
        FourV3 points = ConvertFourV2V3(RectGetPoints(destRect));

        Rect atlasRect = GetAtlasSourceRect(atlas, tileVal);
        FourV4 texCoordPoints = ConvertFourV2V4(GetTextureRectTexCoords(atlas.texture, atlasRect));

        u32 indexBase = vertsAdded;
        for (int i = 0; i < 4; i++)
        {
            positions[vertsAdded] = points.e[i];
            texCoords[vertsAdded] = texCoordPoints.e[i];
            colFg[vertsAdded] = GetColorV4(colorsFg[tileI]);
            colBg[vertsAdded] = GetColorV4(colorsBg[tileI]);
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
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colBg, vertCount, sizeof(colBg[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_TEXCOORDS, MakeVertexCountedData(texCoords, vertCount, sizeof(texCoords[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colFg, vertCount, sizeof(colFg[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    BindTextureSlot(0, atlas.texture);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    UnbindTextureSlot(0);

    MemoryArenaUnfreeze(arena);
}

api_func void DrawFilledCircle(
    v2 center,
    f32 radius,
    SavColor color,
    MemoryArena *arena,
    int segments)
{
    int vertCount = segments + 1;
    int indexCount = segments * 3;

    MemoryArenaFreeze(arena);

    v3 *positions = MemoryArenaPushArrayAndZero(arena, vertCount, v3);
    v4 *colors = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(arena, indexCount, u32);

    int vertsAdded = 0;
    int indicesAdded = 0;
    f32 angleDelta = (f32) PI * 2.0f / segments;

    f32 currentAngle = 0.0f;

    positions[vertsAdded] = V3(center);
    colors[vertsAdded] = GetColorV4(color);
    vertsAdded++;
    positions[vertsAdded] = V3(center + radius * GetUnitVecFromAngle(currentAngle));
    colors[vertsAdded] = GetColorV4(color);
    vertsAdded++;

    for (int segmentI = 0; segmentI < segments - 1; segmentI++)
    {
        currentAngle += angleDelta;

        positions[vertsAdded] = V3(center + radius * GetUnitVecFromAngle(currentAngle));
        colors[vertsAdded] = GetColorV4(color);

        indices[indicesAdded++] = 0;
        indices[indicesAdded++] = vertsAdded;
        indices[indicesAdded++] = vertsAdded - 1;

        vertsAdded++;
    }

    indices[indicesAdded++] = 0;
    indices[indicesAdded++] = vertsAdded - 1;
    indices[indicesAdded++] = 1;

    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);
    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    MemoryArenaUnfreeze(arena);
}

api_func void DrawFilledCircleSegment(
    v2 center,
    f32 radius,
    f32 startDeg,
    f32 endDeg,
    SavColor color,
    int segments,
    MemoryArena *arena)
{
    if (startDeg > endDeg)
    {
        f32 temp = startDeg;
        startDeg = endDeg;
        endDeg = temp;
    }

    while (endDeg - startDeg > 360.0f)
    {
        endDeg = startDeg + 360.0f;
    }

    int vertCount = segments + 2;
    int indexCount = segments * 3;

    MemoryArenaFreeze(arena);

    v3 *positions = MemoryArenaPushArrayAndZero(arena, vertCount, v3);
    v4 *colors = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(arena, indexCount, u32);

    int vertsAdded = 0;
    int indicesAdded = 0;
    f32 angleDelta = ToRad(endDeg - startDeg) / segments;
    f32 currentAngle = ToRad(startDeg);

    positions[vertsAdded] = V3(center);
    colors[vertsAdded] = GetColorV4(color);
    vertsAdded++;
    positions[vertsAdded] = V3(center + radius * GetUnitVecFromAngleFlipY(currentAngle));
    colors[vertsAdded] = GetColorV4(color);
    vertsAdded++;

    for (int segmentI = 0; segmentI < segments; segmentI++)
    {
        currentAngle += angleDelta;

        positions[vertsAdded] = V3(center + radius * GetUnitVecFromAngleFlipY(currentAngle));
        colors[vertsAdded] = GetColorV4(color);

        indices[indicesAdded++] = 0;
        indices[indicesAdded++] = vertsAdded;
        indices[indicesAdded++] = vertsAdded - 1;

        vertsAdded++;
    }

    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);
    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    MemoryArenaUnfreeze(arena);
}

api_func void DrawFilledCircleSegment(
    v2 center,
    f32 radius,
    f32 startDeg,
    f32 endDeg,
    SavColor color,
    MemoryArena *arena)
{
    f32 deltaDeg = endDeg - startDeg;
    if (deltaDeg > 360.0f) deltaDeg = 360.0f;

    int segments = RoundF32ToI32(deltaDeg / 22.5f);

    DrawFilledCircleSegment(
        center,
        radius,
        startDeg,
        endDeg,
        color,
        segments,
        arena);
}

api_func void DrawLine(
    v2 start,
    v2 end,
    SavColor color)
{
    v2 vec = end - start;
    v3 perp = V3(-vec.y, vec.x, 0.0f);
    perp = VecNormalizeIN0(perp);

    v3 positions[3];
    positions[0] = V3(start);
    positions[1] = V3(end) - perp;
    positions[2] = V3(end) + perp;
    v4 colorV4 = GetColorV4(color);
    v4 colors[] = { colorV4, colorV4, colorV4 };

    u32 indices[] = { 0, 1, 2 };

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, ArrayCount(positions), ArrayCount(indices));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, ArrayCount(positions), sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, ArrayCount(positions), sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, ArrayCount(indices), sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
}
