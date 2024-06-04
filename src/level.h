#ifndef LEVEL_H
#define LEVEL_H

#include "sav.h"

struct Tile
{
    i32 atlasValue;
    SavColor fg;
    SavColor bg;
};

struct Level
{
    int w, h;
    Tile *tiles;

    f32 tilePxW, tilePxH;
    SavTextureAtlas *atlas;
    MemoryArena *arena;
};

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale);
api_func void UpdateLevel(Level *level);
api_func void DrawLevel(Level *gameState);

#endif