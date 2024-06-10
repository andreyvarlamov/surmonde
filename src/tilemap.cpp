#include "tilemap.h"
#include "sav.h"
#include "draw.h"

api_func Tilemap MakeTilemap(MemoryArena *arena, SavTextureAtlas *atlas, f32 tilePxW, f32 tilePxH, int w, int h)
{
    Tilemap tilemap;
    tilemap.arena = arena;
    tilemap.atlas = atlas;
    tilemap.tilePxW = tilePxW;
    tilemap.tilePxH = tilePxH;

    tilemap.w = w;
    tilemap.h = h;
    int count = w * h;
    tilemap.atlasValues = MemoryArenaPushArrayAndZero(arena, count, i32);
    tilemap.colorsBg = MemoryArenaPushArrayAndZero(arena, count, SavColor);
    tilemap.colorsFg = MemoryArenaPushArrayAndZero(arena, count, SavColor);
    // for (int i = 0; i < count; i++)
    // {
    //     tilemap.atlasValues[i] = '!';
    //     tilemap.colorsBg[i] = SAV_COLOR_RED;
    //     tilemap.colorsFg[i] = SAV_COLOR_YELLOW;
    // }
    return tilemap;
}

api_func Tile MakeTile(i32 atlasValue, SavColor bg, SavColor fg)
{
    Tile tile;
    tile.atlasValue = atlasValue;
    tile.bg = bg;
    tile.fg = fg;
    return tile;
}

api_func Tile GetTile(Tilemap *tilemap, int i)
{
    Assert(i >= 0 && i < tilemap->w * tilemap->h);
    Tile tile;
    tile.atlasValue = tilemap->atlasValues[i];
    tile.bg = tilemap->colorsBg[i];
    tile.fg = tilemap->colorsFg[i];
    return tile;
}

api_func Tile GetTile(Tilemap *tilemap, int x, int y)
{
    Assert(x >= 0 && x < tilemap->w && y >= 0 && y <= tilemap->h);
    int i = XYToIdx(tilemap->w, x, y);
    Tile tile;
    tile.atlasValue = tilemap->atlasValues[i];
    tile.bg = tilemap->colorsBg[i];
    tile.fg = tilemap->colorsFg[i];
    return tile;
}

api_func void SetTile(Tilemap *tilemap, int i, Tile tile)
{
    Assert(i >= 0 && i < tilemap->w * tilemap->h);
    tilemap->atlasValues[i] = tile.atlasValue;
    tilemap->colorsBg[i] = tile.bg;
    tilemap->colorsFg[i] = tile.fg;
}

api_func void SetTile(Tilemap *tilemap, int x, int y, Tile tile)
{
    Assert(x >= 0 && x < tilemap->w && y >= 0 && y <= tilemap->h);
    int i = XYToIdx(tilemap->w, x, y);
    tilemap->atlasValues[i] = tile.atlasValue;
    tilemap->colorsBg[i] = tile.bg;
    tilemap->colorsFg[i] = tile.fg;
}

api_func void DrawTilemap(Tilemap *tilemap, v2 origin)
{
    DrawAtlasTilemap(
        *tilemap->atlas,
        tilemap->w,
        tilemap->h,
        tilemap->tilePxW,
        tilemap->tilePxH,
        tilemap->atlasValues,
        tilemap->colorsFg,
        tilemap->colorsBg,
        origin,
        tilemap->arena);
}