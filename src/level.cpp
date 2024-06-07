#include "level.h"
#include "sav.h"
#include "settings.h"
#include "draw.h"
#include "helpers.h"
#include "entity.h"
#include "tilemap.h"

api_func Level MakeLevel(
    int w, int h,
    SavTextureAtlas *atlas,
    f32 tilePxW, f32 tilePxH,
    int entityMax,
    MemoryArena *arena)
{
    Level level;
    level.w = w;
    level.h = h;
    level.levelTilemap = MakeTilemap(arena, atlas, tilePxW, tilePxH, w, h);
    if (entityMax > 0)
    {
        level.entityStore = MakeEntityStore(arena, entityMax, w, h);
        level.entityTilemap = MakeTilemap(arena, atlas, tilePxW, tilePxH, w, h);
    }
    level.arena = arena;
    return level;
}

internal_func void generateLevelOneRoom(Level *level)
{
    Tile floorTile = MakeTile('.', SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT);
    Tile wallTile = MakeTile('#', SAV_COLOR_SABLE, SAV_COLOR_OIL);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(&level->levelTilemap, i, floorTile);
    }

    for (int x = 0; x < level->w; x++)
    {
        SetTile(&level->levelTilemap, x, 0, wallTile);
        SetTile(&level->levelTilemap, x, level->h - 1, wallTile);
    }

    for (int y = 0; y < level->h; y++)
    {
        SetTile(&level->levelTilemap, 0, y, wallTile);
        SetTile(&level->levelTilemap, level->w - 1, y, wallTile);
    }
}

api_func void GenerateLevel(Level *level, LevelGenType genType)
{
    switch (genType)
    {
        case LEVEL_ONE_ROOM:
        {
            generateLevelOneRoom(level);
        } break;

        case LEVEL_CLASSIC_ROOMS:
        {
            InvalidCodePath;
        } break;

        default: InvalidCodePath;
    }

    Entity playerEntity = MakeEntity(5, 5, level, MakeTile('@', SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY));
    AddEntity(&level->entityStore, playerEntity);
}

api_func void UpdateLevel(Level *level)
{
    // UpdateEntities

    PopulateTilemap(&level->entityStore, &level->entityTilemap);
}

api_func void DrawLevel(Level *level)
{
    DrawTilemap(&level->levelTilemap);
    DrawTilemap(&level->entityTilemap);
}

// TODO: tile px dim shouldn't be part of level
api_func v2 GetLevelTilePxDim(Level *level)
{
    v2 dim = V2(level->levelTilemap.tilePxW, level->levelTilemap.tilePxH);
    return dim;
}