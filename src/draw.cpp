#include "draw.h"
#include "sav.h"
#include "defines.h"

api_func void DrawAtlasTilemap(
    SavTextureAtlas atlas,
    int tilemapWidth,
    int tilemapHeight,
    f32 tilePxW,
    f32 tilePxH,
    i32 *atlasValues,
    v4 *colors,
    v2 origin,
    MemoryArena *arena)
{
    int tileCount = tilemapWidth * tilemapHeight;
    int vertCount = tileCount * 4;
    int indexCount = tileCount * 6;

    MemoryArenaFreeze(arena);

    v3 *positions = MemoryArenaPushArrayAndZero(arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    v4 *vertColors = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
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
            vertColors[vertsAdded] = colors[tileI];
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
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(vertColors, vertCount, sizeof(vertColors[0])));
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

api_func void DrawBadLine(
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

api_func void DrawFilledPolygon(v2 *verts, int vertCount, SavColor color)
{
    Assert(vertCount > 2);
    
    MemoryArena *scratch = MemoryArenaScratch(NULL);

    v3 *positions = MemoryArenaPushArray(scratch, vertCount, v3);
    v4 *colors = MemoryArenaPushArray(scratch, vertCount, v4);

    v4 colorV4 = GetColorV4(color);
    
    for (int i = 0; i < vertCount; i++)
    {
        positions[i] = V3(verts[i]);
        colors[i] = colorV4;
    }

    int indexTotal = (vertCount - 2) * 3;
    u32 *indices = MemoryArenaPushArray(scratch, indexTotal, u32);
    int indexCount = 0;

    for (int vertIndex = 0; vertIndex  < vertCount - 1; vertIndex++)
    {
        if (vertIndex == 0)
        {
            indices[indexCount++] = 0;
            indices[indexCount++] = 1;
            indices[indexCount++] = 2;
        }
        else if (vertIndex == 1)
        {
            continue;
        }
        else
        {
            indices[indexCount++] = 0;
            indices[indexCount++] = vertIndex;
            indices[indexCount++] = vertIndex + 1;
        }
    }

    Assert(indexCount == indexTotal);

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexTotal);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexTotal, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
}


struct FloaterState
{
    SavFont *font;
    Camera2D *camera;
    MemoryArena *arena;

    Floater floaters[FLOATERS_MAX];
};

global_var FloaterState _floaterState; // TODO: Do this better

api_func void InitializeFloaterState(SavFont *font, Camera2D* camera, MemoryArena *arena)
{
    _floaterState.font = font;
    _floaterState.camera = camera;
    _floaterState.arena = arena;
}

api_func void AddFloater(const char *string, f32 pointSize, SavColor color, v2 worldP)
{
    for (int i = 0; i < FLOATERS_MAX; i++)
    {
        Floater *floater = _floaterState.floaters + i;
        if (!floater->isUsed)
        {
            floater->isUsed = true;
            strcpy(floater->string, string);
            floater->pointSize = pointSize;
            floater->color = color;
            floater->startingWorldP = V2(worldP.x * 32.0f, worldP.y * 32.0f);
            floater->yOffset = 0.0f;
            floater->opacity = 1.0f;
            return;
        }
    }

    InvalidCodePath; // Not enough floater slots
}

api_func void DrawFloaters(f32 delta)
{
    for (int i = 0; i < FLOATERS_MAX; i++)
    {
        Floater *floater = _floaterState.floaters + i;
        if (floater->isUsed)
        {
            SavColor color = ColorAlpha(floater->color, floater->opacity);

            v2 screenP = CameraWorldToScreen(_floaterState.camera, floater->startingWorldP);
            DrawString(floater->string, _floaterState.font, floater->pointSize, color, screenP.x, screenP.y - floater->yOffset, 0.0f, _floaterState.arena);

            floater->yOffset += 100.0f * delta;
            floater->opacity -= 0.3f * delta;

            if (floater->yOffset > 1000.0f || floater->opacity < 0.0f)
            {
                floater->isUsed = false;
            }
        }
    }
}
