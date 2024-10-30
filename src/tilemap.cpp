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
    tilemap.colors = MemoryArenaPushArrayAndZero(arena, count, v4);
    return tilemap;
}

api_func TileSprite MakeTileSprite(int atlasValue, v4 color)
{
    TileSprite tileSprite;
    tileSprite.atlasValue = atlasValue;
    tileSprite.color = color;
    return tileSprite;
}

api_func TileSprite GetTileSprite(Tilemap *tilemap, int i)
{
    Assert(i >= 0 && i < tilemap->w * tilemap->h);
    TileSprite tileSprite;
    tileSprite.atlasValue = tilemap->atlasValues[i];
    tileSprite.color = tilemap->colors[i];
    return tileSprite;
}

api_func TileSprite GetTileSprite(Tilemap *tilemap, int x, int y)
{
    Assert(x >= 0 && x < tilemap->w && y >= 0 && y <= tilemap->h);
    int i = XYToIdx(tilemap->w, x, y);
    TileSprite tileSprite;
    tileSprite.atlasValue = tilemap->atlasValues[i];
    tileSprite.color = tilemap->colors[i];
    return tileSprite;
}

api_func void SetTileSprite(Tilemap *tilemap, int i, TileSprite tileSprite)
{
    Assert(i >= 0 && i < tilemap->w * tilemap->h);
    tilemap->atlasValues[i] = tileSprite.atlasValue;
    tilemap->colors[i] = tileSprite.color;
}

api_func void SetTileSprite(Tilemap *tilemap, int x, int y, TileSprite tileSprite)
{
    Assert(x >= 0 && x < tilemap->w && y >= 0 && y <= tilemap->h);
    int i = XYToIdx(tilemap->w, x, y);
    tilemap->atlasValues[i] = tileSprite.atlasValue;
    tilemap->colors[i] = tileSprite.color;
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
        tilemap->colors,
        origin,
        tilemap->arena);
}
