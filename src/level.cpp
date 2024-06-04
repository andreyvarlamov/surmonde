#include "level.h"
#include "sav.h"
#include "settings.h"

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale)
{
    Level level;
    level.w = w;
    level.h = h;
    level.tiles = MemoryArenaPushArrayAndZero(mem, w * h, Tile);
    for (int i = 0; i < level.w * level.h; i++)
    {
        level.tiles[i].bg = SAV_COLOR_SABLE;
    }
    level.tilePxW = atlas->cellW * atlasScale;
    level.tilePxH = atlas->cellH * atlasScale;
    level.atlas = atlas;
    level.arena = mem;
    return level;
}

api_func void UpdateLevel(Level *level)
{
    for (int i = 0; i < level->w * level->h; i++)
    {
        Tile *tile = level->tiles + i;
        if (GetCurrentFrame() % 5 == 0)
        {
            if (GetRandomValue(0, 10) == 0)
            {
                tile->atlasValue = GetRandomValue(0, 256);
                // bg, fg
                // SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY
                // SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT
                // SAV_COLOR_SABLE, SAV_COLOR_LIGHTGOLDENRODYELLOW
                tile->fg = SAV_COLOR_ASHGRAY;
                tile->bg = SAV_COLOR_SABLE;
                // tile->fg = MakeColor((u8)GetRandomValue(0, 256), (u8)GetRandomValue(0, 256), (u8)GetRandomValue(0, 256), 255);
                // tile->bg = MakeColor((u8)GetRandomValue(0, 256), (u8)GetRandomValue(0, 256), (u8)GetRandomValue(0, 256), 255);
            }
        }
    }
}

api_func void DrawLevel(Level *level)
{
    int tileCount = level->w * level->h;
    int vertCount = tileCount * 4;
    int indexCount = tileCount * 6;

    MemoryArenaFreeze(level->arena);

    v3 *positions = MemoryArenaPushArrayAndZero(level->arena, vertCount, v3);
    v4 *texCoords = MemoryArenaPushArrayAndZero(level->arena, vertCount, v4);
    v4 *colorsFg = MemoryArenaPushArrayAndZero(level->arena, vertCount, v4);
    v4 *colorsBg = MemoryArenaPushArrayAndZero(level->arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(level->arena, indexCount, u32);
    int vertsAdded = 0;
    int indicesAdded = 0;
    for (int tileI = 0; tileI < level->w * level->h; tileI++)
    {
        Tile *tile = level->tiles + tileI;
        i32 tileVal = tile->atlasValue;
        i32 tileX = tileI % level->w;
        i32 tileY = tileI / level->h;
        Rect destRect = MakeRect(tileX * level->tilePxW, tileY * level->tilePxH, level->tilePxW, level->tilePxH);
        int atlasPxX = (tileVal % level->atlas->cellHorizontalCount) * level->atlas->cellW;
        int atlasPxY = (tileVal / level->atlas->cellHorizontalCount) * level->atlas->cellH;
        Rect atlasRect = MakeRect((f32) atlasPxX, (f32) atlasPxY, (f32) level->atlas->cellW, (f32) level->atlas->cellH);

        FourV3 points = ConvertFourV2V3(RectGetPoints(destRect));
        FourV4 texCoordPoints = ConvertFourV2V4(GetTextureRectTexCoords(level->atlas->texture, atlasRect));

        u32 indexBase = vertsAdded;
        for (int i = 0; i < 4; i++)
        {
            positions[vertsAdded] = points.e[i];
            texCoords[vertsAdded] = texCoordPoints.e[i];
            colorsFg[vertsAdded] = GetColorV4(tile->fg);
            colorsBg[vertsAdded] = GetColorV4(tile->bg);
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
    BindTextureSlot(0, level->atlas->texture);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);
    UnbindTextureSlot(0);

    MemoryArenaUnfreeze(level->arena);
}