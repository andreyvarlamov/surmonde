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
