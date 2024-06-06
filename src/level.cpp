#include "level.h"
#include "sav.h"
#include "settings.h"
#include "draw.h"
#include "helpers.h"

internal_func Tiles makeTiles(MemoryArena *mem, int w, int h)
{
    int count = w * h;
    Tiles tiles;
    tiles.w = w;
    tiles.h = h;
    tiles.atlasValues = MemoryArenaPushArray(mem, count, i32);
    tiles.colorsFg = MemoryArenaPushArray(mem, count, SavColor);
    tiles.colorsBg = MemoryArenaPushArray(mem, count, SavColor);
    for (int i = 0; i < count; i++)
    {
        tiles.atlasValues[i] = '!';
        tiles.colorsFg[i] = SAV_COLOR_YELLOW;
        tiles.colorsBg[i] = SAV_COLOR_RED;
    }
    return tiles;
}

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale)
{
    Level level;
    level.w = w;
    level.h = h;
    level.mapTiles = makeTiles(mem, w, h);
    level.tilePxW = atlas->cellW * atlasScale;
    level.tilePxH = atlas->cellH * atlasScale;
    level.atlas = atlas;
    level.arena = mem;
    return level;
}

internal_func Tile getTile(Tiles *tiles, int i)
{
    Tile tile;
    tile.atlasValue = tiles->atlasValues[i];
    tile.fg = tiles->colorsFg[i];
    tile.bg = tiles->colorsBg[i];
    return tile;
}

internal_func void setTile(Tiles *tiles, Tile tile, int i)
{
    tiles->atlasValues[i] = tile.atlasValue;
    tiles->colorsFg[i] = tile.fg;
    tiles->colorsBg[i] = tile.bg;
}

internal_func Tile makeTile(i32 atlasValue, SavColor bg, SavColor fg)
{
    Tile tile;
    tile.atlasValue = atlasValue;
    tile.fg = fg;
    tile.bg = bg;
    return tile;
}

internal_func void generateLevelOneRoom(Level *level)
{
    Tile floorTile = makeTile('.', SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT);
    Tile wallTile = makeTile('#', SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY);
    for (int i = 0; i < level->w * level->h; i++)
    {
        setTile(&level->mapTiles, floorTile, i);
    }

    for (int x = 0; x < level->w; x++)
    {
        int i = x;
        setTile(&level->mapTiles, wallTile, i);
        i = XYToIdx(level->w, x, level->h - 1);
        setTile(&level->mapTiles, wallTile, i);
    }

    for (int y = 0; y < level->h; y++)
    {
        int i = XYToIdx(level->w, 0, y);
        setTile(&level->mapTiles, wallTile, i);
        i = XYToIdx(level->w, level->w - 1, y);
        setTile(&level->mapTiles, wallTile, i);
    }
}

api_func void GenerateLevel(Level *level, LevelGenType genType)
{
    switch (genType)
    {
        case LEVEL_ONE_ROOM:
        {
            generateLevelOneRoom(level);
        } break;

        case LEVEL_CLASSIC_ROOMS:
        {
            InvalidCodePath;
        } break;

        default: InvalidCodePath;
    }
}

api_func void UpdateLevel(Level *level)
{
}

api_func void DrawLevel(Level *level)
{
    DrawAtlasTilemap(
        *level->atlas,
        level->w,
        level->h,
        level->tilePxW,
        level->tilePxH,
        level->mapTiles.atlasValues,
        level->mapTiles.colorsFg,
        level->mapTiles.colorsBg,
        level->arena);
}

api_func v2 GetLevelTilePxDim(Level *level)
{
    v2 dim = V2(level->tilePxW, level->tilePxH);
    return dim;
}