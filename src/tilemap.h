#ifndef TILEMAP_H
#define TILEMAP_H

#include "sav.h"

struct TileSprite
{
    i32 atlasValue;
    SavColor fg;
    SavColor bg;
};

struct Tilemap
{
    MemoryArena *arena;
    SavTextureAtlas *atlas;
    f32 tilePxW, tilePxH;

    int w, h;
    i32 *atlasValues;
    SavColor *colorsFg;
    SavColor *colorsBg;
};

api_func Tilemap MakeTilemap(MemoryArena *arena, SavTextureAtlas *atlas, f32 tilePxW, f32 tilePxH, int w, int h);
api_func TileSprite MakeTileSprite(i32 atlasValue, SavColor bg, SavColor fg);
api_func TileSprite GetTileSprite(Tilemap *tilemap, int i);
api_func TileSprite GetTileSprite(Tilemap *tilemap, int x, int y);
api_func void SetTileSprite(Tilemap *tilemap, int i, TileSprite tileSprite);
api_func void SetTileSprite(Tilemap *tilemap, int x, int y, TileSprite tileSprite);
api_func void DrawTilemap(Tilemap *tilemap, v2 origin);

#endif
