#ifndef LEVEL_H
#define LEVEL_H

#include "sav.h"

struct Tile
{
    i32 atlasValue;
    SavColor fg;
    SavColor bg;
};

struct Tiles
{
    int w;
    int h;
    i32 *atlasValues;
    SavColor *colorsFg;
    SavColor *colorsBg;
};

struct Level
{
    int w, h;
    Tiles mapTiles;

    f32 tilePxW, tilePxH;
    SavTextureAtlas *atlas;
    MemoryArena *arena;
};

enum LevelGenType
{
    LEVEL_ONE_ROOM,
    LEVEL_CLASSIC_ROOMS
};

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale);
api_func void GenerateLevel(Level *level, LevelGenType genType);
api_func void UpdateLevel(Level *level);
api_func void DrawLevel(Level *level);
api_func v2 GetLevelTilePxDim(Level *level);

#endif