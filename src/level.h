#ifndef LEVEL_H
#define LEVEL_H

#include "sav.h"

struct Level
{
    int w, h;
    i32 *tiles;

    f32 tilePxW, tilePxH;
    SavTextureAtlas *atlas;
};

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale);
api_func void DrawLevel(Level *gameState);

#endif