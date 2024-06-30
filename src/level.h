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

struct MeshVert
{
    v2 p;
};

struct MeshEdge
{
    MeshVert *verts[2];
};

struct MeshTile
{
    MeshEdge *edges[4];
};

struct LevelMesh
{
    MeshVert *verts;
    int vertCount;

    MeshEdge *edges;
    int edgeCount;

    MeshTile *tiles;
    int w, h;
};

struct Level
{
    int w, h;
    Tilemap levelTilemap;
    u8 *tileFlags;

    LevelMesh mesh;

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
api_func void PreprocessLevel(Level *level);
api_func void UpdateLevel(Level *level);
api_func void DrawLevel(Level *level);

api_func Tile MakeTile(i32 atlasValue, SavColor bg, SavColor fg, u8 flags);
api_func void SetTile(Level *level, int i, Tile tile);
api_func void SetTile(Level *level, int x, int y, Tile tile);
api_func b32 IsTileBlocked(Level *level, int x, int y);
api_func b32 IsTileOpaque(Level *level, int x, int y);

api_func void DrawLevelOcclusion(Level *level, v2 pov);
api_func void DebugEdgeOcclusion(Level *level, v2 pov, int i);
api_func void DrawLevelMeshDebug(Level *level);

#endif
