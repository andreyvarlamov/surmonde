#ifndef LEVEL_H
#define LEVEL_H

#include "sav.h"
#include "tilemap.h"
#include "entity.h"

enum TileFlags
{
    TileFlags_Blocked = 0x1,
    TileFlags_Opaque = 0x2,
    TileFlags_Apron = 0x4
};

struct Tile
{
    TileSprite sprite;
    u8 flags;
};

struct Level
{
    v2i worldPos;
    
    int w, h;
    Tilemap levelTilemap;
    u8 *tileFlags;

    MemoryArena *arena;
};

enum LevelGenType
{
    LevelGenType_Empty,
    LevelGenType_OneRoom,
    LevelGenType_ClassicRooms
};

struct LevelStore
{
    i32 standardLevelWidth;
    i32 standardLevelHeight;
    SavTextureAtlas worldAtlas;
    f32 tilePxW;
    f32 tilePxH;
    MemoryArena *arena;

    int maxLevelCount;
    int levelCount;
    v2i levelWorldPositions[LEVEL_STORE_MAX_LEVELS];
    Level levels[LEVEL_STORE_MAX_LEVELS];

    Level *currentLevel;
};

api_func LevelStore MakeLevelStore(i32 standardLevelWidth,
                                   i32 standardLevelHeight,
                                   SavTextureAtlas atlas,
                                   f32 tilePxW,
                                   f32 tilePxH,
                                   MemoryArena *arena,
                                   int maxLevelCount);

api_func Level *GetLevelAtWorldPos(LevelStore *s, EntityStore *entityStore, v2i worldPos);
api_func Level *GetCurrentLevel(LevelStore *s);
api_func void SetCurrentLevel(LevelStore *s, Level *level);

api_func Level MakeLevel(v2i worldPos,
                         int w, int h,
			 SavTextureAtlas *atlas,
			 f32 tilePxW, f32 tilePxH,
			 MemoryArena *arena);
api_func void GenerateLevel(Level *level, EntityStore *entityStore, LevelGenType genType, b32 generateEntities);
api_func void UpdateLevel(Level *level);
api_func void DrawLevel(Level *level);
api_func void DrawLevelOcclusion(Level *level, u8 *visibleTiles);

api_func Tile MakeTile(i32 atlasValue, v4 color, u8 flags);
api_func void SetTile(Level *level, int i, Tile tile);
api_func void SetTile(Level *level, int x, int y, Tile tile);
api_func b32 IsTileBlocked(Level *level, int x, int y);
api_func b32 IsTileOpaque(Level *level, int x, int y);
api_func Direction4 IsTileApron(Level *level, int x, int y);

#endif
