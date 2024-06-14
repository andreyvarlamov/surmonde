#include "level.h"
#include "sav.h"
#include "settings.h"
#include "draw.h"
#include "helpers.h"
#include "tilemap.h"
#include "entity.h"

api_func Level MakeLevel(
    int w, int h,
    SavTextureAtlas *atlas,
    f32 tilePxW, f32 tilePxH,
    MemoryArena *arena)
{
    Level level;
    level.w = w;
    level.h = h;
    level.levelTilemap = MakeTilemap(arena, atlas, tilePxW, tilePxH, w, h);
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

api_func void GenerateLevel(Level *level, EntityStore *entityStore, LevelGenType genType)
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

    // TODO: Maybe don't do this inside level gen
    CharacterStats stats = {};
    stats.viewRadius = 10.0f;
    stats.attackReach = 1.0f;
    stats.speed = 10.0f;
    Entity playerEntity = MakeEntity(5.0f, 5.0f, level, '@', SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY);
    ConfigureCharacterEntity(&playerEntity, stats);
    Entity *addedEntity = AddEntity(entityStore, playerEntity);
    entityStore->controlledEntity = addedEntity;

    Entity enemyEntity = MakeEntity(30.0f, 30.0f, level, 'E', SAV_COLOR_SABLE, SAV_COLOR_RED);
    ConfigureCharacterEntity(&enemyEntity, stats);
    AddEntity(entityStore, enemyEntity);
}

api_func void DrawLevel(Level *level)
{
    DrawTilemap(&level->levelTilemap, V2(-0.5f, -0.5f));
}

// TODO: tile px dim shouldn't be part of level
api_func v2 GetLevelTilePxDim(Level *level)
{
    v2 dim = V2(level->levelTilemap.tilePxW, level->levelTilemap.tilePxH);
    return dim;
}
