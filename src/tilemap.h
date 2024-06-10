#ifndef TILEMAP_H
#define TILEMAP_H

#include "sav.h"

struct Tile
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
api_func Tile MakeTile(i32 atlasValue, SavColor bg, SavColor fg);
api_func Tile GetTile(Tilemap *tilemap, int i);
api_func Tile GetTile(Tilemap *tilemap, int x, int y);
api_func void SetTile(Tilemap *tilemap, int i, Tile tile);
api_func void SetTile(Tilemap *tilemap, int x, int y, Tile tile);
api_func void DrawTilemap(Tilemap *tilemap, v2 origin);

#endif