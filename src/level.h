#ifndef LEVEL_H
#define LEVEL_H

#include "sav.h"
#include "tilemap.h"
#include "entity.h"

struct Level
{
    int w, h;
    Tilemap levelTilemap;

    MemoryArena *arena;
};

enum LevelGenType
{
    LEVEL_ONE_ROOM,
    LEVEL_CLASSIC_ROOMS
};

api_func Level MakeLevel(int w, int h,
			 SavTextureAtlas *atlas,
			 f32 tilePxW, f32 tilePxH,
			 MemoryArena *arena);
api_func void GenerateLevel(Level *level, EntityStore *entityStore, LevelGenType genType);
api_func void UpdateLevel(Level *level);
api_func void DrawLevel(Level *level);
api_func v2 GetLevelTilePxDim(Level *level);

#endif
