#ifndef LEVEL_H
#define LEVEL_H

#include "sav.h"
#include "tilemap.h"
#include "entity.h"

enum TileFlags
{
    TILE_BLOCKED = 0x1,
    TILE_OPAQUE = 0x2,
};

struct Tile
{
    TileSprite sprite;
    u8 flags;
};

struct Level
{
    int w, h;
    Tilemap levelTilemap;
    u8 *tileFlags;

    MemoryArena *arena;
};

enum LevelGenType
{
    LEVEL_EMPTY,
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
api_func void DrawLevelOcclusion(Level *level, u8 *visibleTiles);

api_func Tile MakeTile(i32 atlasValue, v4 color, u8 flags);
api_func void SetTile(Level *level, int i, Tile tile);
api_func void SetTile(Level *level, int x, int y, Tile tile);
api_func b32 IsTileBlocked(Level *level, int x, int y);
api_func b32 IsTileOpaque(Level *level, int x, int y);

#endif
