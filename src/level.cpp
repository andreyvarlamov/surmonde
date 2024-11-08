#include "level.h"
#include "sav.h"
#include "defines.h"
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
    Level level = {};
    level.w = w;
    level.h = h;
    level.levelTilemap = MakeTilemap(arena, atlas, tilePxW, tilePxH, w, h);
    level.tileFlags = MemoryArenaPushArrayAndZero(arena, w * h, u8);
    level.arena = arena;
    return level;
}

internal_func void generateLevelEmpty(Level *level, v2 *whereToPlacePlayer)
{
    Tile floorTile = MakeTile(4, V4(1,1,1,1), 0);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(level, i, floorTile);
    }

    *whereToPlacePlayer = V2(20.0f, 20.0f);
}

internal_func void generateLevelOneRoom(Level *level, v2 *whereToPlacePlayer)
{
    Tile floorTile = MakeTile(4, V4(1,1,1,1), 0);
    Tile wallTile = MakeTile(3, V4(1,1,1,1), TILE_BLOCKED | TILE_OPAQUE);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(level, i, floorTile);
    }

    for (int x = 0; x < level->w; x++)
    {
        SetTile(level, x, 0, wallTile);
        SetTile(level, x, level->h - 1, wallTile);
    }

    for (int y = 0; y < level->h; y++)
    {
        SetTile(level, 0, y, wallTile);
        SetTile(level, level->w - 1, y, wallTile);
    }

    *whereToPlacePlayer = V2(25.0f, 25.0f);
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

internal_func void generateLevelClassicRooms(Level *level, v2 *whereToPlacePlayer)
{
    Tile grassTile = MakeTile(0, V4(1,1,1,1), 0);
    Tile dirtTile = MakeTile(1, V4(1,1,1,1), 0);
    Tile stonePathTile = MakeTile(2, V4(1,1,1,1), 0);
    Tile wallTile = MakeTile(3, V4(1,1,1,1), TILE_BLOCKED | TILE_OPAQUE);
    
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(level, i, ((GetRandomFloat() > 0.5f) ? grassTile : dirtTile));
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
                    SetTile(level, x, y, stonePathTile);
                }
            }

            for (int y = room.y; y < (room.y + room.h); y++)
            {
                SetTile(level, room.x, y, wallTile);
                SetTile(level, room.x + room.w - 1, y, wallTile);
            }

            for (int x = room.x; x < (room.x + room.w); x++)
            {
                SetTile(level, x, room.y, wallTile);
                SetTile(level, x, room.y + room.h - 1, wallTile);
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
                SetTile(level, x, constY, stonePathTile);
            }
            for (int y = leftCenter.y; y <= rightCenter.y; y++)
            {
                SetTile(level, constX, y, stonePathTile);
            }
        }
        else
        {
            for (int x = leftCenter.x; x <= rightCenter.x; x++)
            {
                SetTile(level, x, constY, stonePathTile);
            }
            for (int y = rightCenter.y; y <= leftCenter.y; y++)
            {
                SetTile(level, constX, y, stonePathTile);
            }
        }
    }

    *whereToPlacePlayer = V2((f32)rooms[0].x + (f32)rooms[0].w * 0.5f, (f32)rooms[0].y + (f32)rooms[0].h * 0.5f);
}

api_func void GenerateLevel(Level *level, EntityStore *entityStore, LevelGenType genType)
{
    v2 playerPos = {};
    switch (genType)
    {
        case LEVEL_EMPTY:
        {
            generateLevelEmpty(level, &playerPos);
        } break;
        
        case LEVEL_ONE_ROOM:
        {
            generateLevelOneRoom(level, &playerPos);
        } break;

        case LEVEL_CLASSIC_ROOMS:
        {
            generateLevelClassicRooms(level, &playerPos);
        } break;

        default: InvalidCodePath;
    }

    // TODO: Maybe don't do this inside level gen
    ActorStats stats = {};
    stats.viewRadius = 30;
    stats.attackReach = 1.0f;
    stats.combatRadius = 2.0f;
    stats.speed = 10.0f;
    stats.health = 100.0f;
    stats.maxHealth = 100.0f;
    Entity playerEntity = MakeEntity(playerPos.x, playerPos.y, level, 0, V4(1,1,1,1), MakeCountedString("Player"), false, false);
    ConfigureActorEntity(&playerEntity, stats);
    Entity *addedEntity = AddEntity(entityStore, playerEntity);
    entityStore->controlledEntity = addedEntity;

    Entity enemyEntity = MakeEntity(30.0f, 30.0f, level, 1, V4(1,1,1,1), MakeCountedString("Enemy"), true, true);
    ConfigureActorEntity(&enemyEntity, stats);
    AddEntity(entityStore, enemyEntity);
}

api_func void DrawLevel(Level *level)
{
    DrawTilemap(&level->levelTilemap, V2(0.0f, 0.0f));
}

api_func void DrawLevelOcclusion(Level *level, u8 *visibleTiles)
{
    MemoryArena *arena = level->arena;
    int allTileCount = level->w * level->h;
    int tileCount = 0;
    for (int i = 0; i < allTileCount; i++)
    {
        if (!visibleTiles[i]) tileCount++;
    }
    int vertCount = tileCount * 4;
    int indexCount = tileCount * 6;
    f32 tilePxW = level->levelTilemap.tilePxW;
    f32 tilePxH = level->levelTilemap.tilePxH;

    MemoryArenaFreeze(arena);

    v3 *positions = MemoryArenaPushArrayAndZero(arena, vertCount, v3);
    v4 *colors = MemoryArenaPushArrayAndZero(arena, vertCount, v4);
    u32 *indices = MemoryArenaPushArrayAndZero(arena, indexCount, u32);
    int vertsAdded = 0;
    int indicesAdded = 0;
    for (int tileI = 0; tileI < allTileCount; tileI++)
    {
        if (!visibleTiles[tileI])
        {
            i32 tileX = tileI % level->w;
            i32 tileY = tileI / level->w;
            f32 pxX = (f32) tileX * tilePxW;
            f32 pxY = (f32) tileY * tilePxH;
            Rect destRect = MakeRect(pxX, pxY, tilePxW, tilePxH);
            FourV3 points = ConvertFourV2V3(RectGetPoints(destRect));

            u32 indexBase = vertsAdded;
            for (int i = 0; i < 4; i++)
            {
                positions[vertsAdded] = points.e[i];
                colors[vertsAdded] = V4(0.0f, 0.0f, 0.0f, 1.0f);
                vertsAdded++;
            }

            u32 localIndices[] = {0, 1, 2, 2, 3, 0};
            for (int i = 0; i < ArrayCount(localIndices); i++)
            {
                indices[indicesAdded++] = indexBase + localIndices[i];
            }
        }
    }

    Assert(vertsAdded == vertCount);
    Assert(indicesAdded == indexCount);

    VertexBatchBeginSub(DEFAULT_VERTEX_BATCH, vertCount, indexCount);
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_POSITIONS, MakeVertexCountedData(positions, vertCount, sizeof(positions[0])));
    VertexBatchSubVertexData(DEFAULT_VERTEX_BATCH, DEFAULT_VERT_COLORS, MakeVertexCountedData(colors, vertCount, sizeof(colors[0])));
    VertexBatchSubIndexData(DEFAULT_VERTEX_BATCH, MakeVertexCountedData(indices, indexCount, sizeof(indices[0])));
    VertexBatchEndSub(DEFAULT_VERTEX_BATCH);
    DrawVertexBatch(DEFAULT_VERTEX_BATCH);

    MemoryArenaUnfreeze(arena);
}

api_func Tile MakeTile(i32 atlasValue, v4 color, u8 flags)
{
    Tile tile;
    tile.sprite = MakeTileSprite(atlasValue, color);
    tile.flags = flags;
    return tile;
}

api_func void SetTile(Level *level, int i, Tile tile)
{
    Assert(i >= 0 && i < level->w * level->h);
    level->tileFlags[i] = tile.flags;
    SetTileSprite(&level->levelTilemap, i, tile.sprite);
}

api_func void SetTile(Level *level, int x, int y, Tile tile)
{
    Assert(x >= 0 && x < level->w && y >= 0 && y <= level->h);
    int i = XYToIdx(level->w, x, y);
    level->tileFlags[i] = tile.flags;
    SetTileSprite(&level->levelTilemap, x, y, tile.sprite);
}

api_func b32 IsTileBlocked(Level *level, int x, int y)
{
    Assert(x >= 0 && x < level->w && y >= 0 && y <= level->h);
    int i = XYToIdx(level->w, x, y);
    b32 result = CheckFlag(level->tileFlags[i], TILE_BLOCKED);
    return result;
}

api_func b32 IsTileOpaque(Level *level, int x, int y)
{
    Assert(x >= 0 && x < level->w && y >= 0 && y <= level->h);
    int i = XYToIdx(level->w, x, y);
    b32 result = CheckFlag(level->tileFlags[i], TILE_OPAQUE);
    return result;
}
