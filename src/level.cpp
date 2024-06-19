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

struct Room
{
    int x, y, w, h;
};

internal_func b32 doRoomsIntersect(Room a, Room b)
{
    b32 result = ((a.x <= b.x + b.w && a.x + a.w >= b.x) &&
                  (a.y <= b.y + b.h && a.y + a.h >= b.y ));
    return result;
}

internal_func void generateLevelClassicRooms(Level *level)
{
    Tile floorTile = MakeTile('.', SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT);
    Tile wallTile = MakeTile('#', SAV_COLOR_SABLE, SAV_COLOR_OIL);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(&level->levelTilemap, i, wallTile);
    }

    int roomsMax = 50;
    int sizeMin = 6;
    int sizeMax = 20;

    Room *rooms = MemoryArenaPushArray(level->arena, roomsMax, Room);

    int roomCount = 0;
    for (int i = 0; i < roomsMax; i++)
    {
        Room room;
        room.w = GetRandomValue(sizeMin, sizeMax);
        room.h = GetRandomValue(sizeMin, sizeMax);
        room.x = GetRandomValue(2, level->w - room.w - 2);
        room.y = GetRandomValue(2, level->h - room.h - 2);

        b32 intersects = false;
        for (int j = 0; j < roomCount; j++)
        {
            if (doRoomsIntersect(room, rooms[j]))
            {
                intersects = true;
                break;
            }
        }

        if (!intersects)
        {
            for (int y = room.y; y < (room.y + room.h); y++)
            {
                for (int x = room.x; x < (room.x + room.w); x++)
                {
                    SetTile(&level->levelTilemap, x, y, floorTile);
                }
            }
            
            rooms[roomCount++] = room;
        }
    }

    for (int i = 0; i < roomCount - 1; i++)
    {
        Room *roomA = rooms + i;
        Room *roomB = rooms + i + 1;

        v2i aCenter = V2I(roomA->x + roomA->w / 2, roomA->y + roomA->h / 2);
        v2i bCenter = V2I(roomB->x + roomB->w / 2, roomB->y + roomB->h / 2);

        v2i leftCenter = aCenter.x < bCenter.x ? aCenter : bCenter;
        v2i rightCenter = aCenter.x >= bCenter.x ? aCenter : bCenter;

        int corridorVersion = GetRandomValue(0, 2);

        int constX;
        int constY;

        if (corridorVersion)
        {
            constX = rightCenter.x;
            constY = leftCenter.y;
        }
        else
        {
            constX = leftCenter.x;
            constY = rightCenter.y;
        }

        if (leftCenter.y < rightCenter.y)
        {
            for (int x = leftCenter.x; x <= rightCenter.x; x++)
            {
                SetTile(&level->levelTilemap, x, constY, floorTile);
            }
            for (int y = leftCenter.y; y <= rightCenter.y; y++)
            {
                SetTile(&level->levelTilemap, constX, y, floorTile);
            }
        }
        else
        {
            for (int x = leftCenter.x; x <= rightCenter.x; x++)
            {
                SetTile(&level->levelTilemap, x, constY, floorTile);
            }
            for (int y = rightCenter.y; y <= leftCenter.y; y++)
            {
                SetTile(&level->levelTilemap, constX, y, floorTile);
            }
        }
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
            generateLevelClassicRooms(level);
        } break;

        default: InvalidCodePath;
    }

    // TODO: Maybe don't do this inside level gen
    CharacterStats stats = {};
    stats.viewRadius = 10.0f;
    stats.attackReach = 1.0f;
    stats.combatRadius = 2.0f;
    stats.speed = 10.0f;
    stats.initiative = 10.0f;
    Entity playerEntity = MakeEntity(25.0f, 20.0f, level, '@', SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY);
    ConfigureCharacterEntity(&playerEntity, stats);
    Entity *addedEntity = AddEntity(entityStore, playerEntity);
    entityStore->controlledEntity = addedEntity;

    stats.initiative = 15.0f;
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
