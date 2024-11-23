#include "level.h"
#include "sav.h"
#include "defines.h"
#include "draw.h"
#include "helpers.h"
#include "tilemap.h"
#include "entity.h"

api_func LevelStore MakeLevelStore(i32 standardLevelWidth,
                                   i32 standardLevelHeight,
                                   SavTextureAtlas atlas,
                                   f32 tilePxW,
                                   f32 tilePxH,
                                   MemoryArena *arena,
                                   int maxLevelCount)
{
    LevelStore s = {};

    // TODO: This probably shouldn't store the atlas, or tile px dim, or the arena
    
    s.standardLevelWidth = standardLevelWidth;
    s.standardLevelHeight = standardLevelHeight;
    s.worldAtlas = atlas;
    s.tilePxW = tilePxW;
    s.tilePxH = tilePxH;
    s.arena = arena;
    s.maxLevelCount = maxLevelCount;
    
    return s;
}

api_func Level MakeLevel(v2i worldPos,
                         int w, int h,
                         SavTextureAtlas *atlas,
                         f32 tilePxW, f32 tilePxH,
                         MemoryArena *arena)
{
    Level level = {};
    level.worldPos = worldPos;
    level.w = w;
    level.h = h;
    level.levelTilemap = MakeTilemap(arena, atlas, tilePxW, tilePxH, w, h);
    level.tileFlags = MemoryArenaPushArrayAndZero(arena, w * h, u8);

    // TODO: Shouldn't keep arenas from outside the level system inside the level
    level.arena = arena;
    
    return level;
}

api_func Level *GetLevelAtWorldPos(LevelStore *s, EntityStore *entityStore, v2i worldPos)
{
    for (int i = 0; i < s->levelCount; i++)
    {
        if (s->levelWorldPositions[i] == worldPos)
        {
            return s->levels + i;
        }
    }

    // Level was not found, create a new one
    
    Assert(s->levelCount < s->maxLevelCount); // Out of level storage
        
    Level *level = s->levels + s->levelCount++;

    *level = MakeLevel(worldPos, s->standardLevelWidth, s->standardLevelHeight, &s->worldAtlas, s->tilePxW, s->tilePxH, s->arena);

    // Decide whether to add entities based on just whether it's world origin for now
    b32 willAddEntities = (worldPos == V2I(0,0));
    GenerateLevel(level, entityStore, LevelGenType_ClassicRooms, willAddEntities);
        
    return level;
}

api_func Level *GetCurrentLevel(LevelStore *s)
{
    return s->currentLevel;
}

api_func void SetCurrentLevel(LevelStore *s, Level *level)
{
    s->currentLevel = level;
}

internal_func void generateLevelEmpty(Level *level, v2 *whereToPlacePlayer)
{
    InvalidCodePath; // not up to date
    
    Tile floorTile = MakeTile(4, V4(1,1,1,1), 0);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(level, i, floorTile);
    }

    *whereToPlacePlayer = V2(20.0f, 20.0f);
}

internal_func void generateLevelOneRoom(Level *level, v2 *whereToPlacePlayer)
{
    InvalidCodePath; // not up to date
    
    Tile floorTile = MakeTile(4, V4(1,1,1,1), 0);
    Tile wallTile = MakeTile(3, V4(1,1,1,1), TileFlags_Blocked | TileFlags_Opaque);
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

internal_func void generateLevelClassicRooms(Level *level, v2 *whereToPlacePlayer, v2 *whereToPlaceChest, v2 *whereToPlaceCampfire)
{
    Tile grassTile = MakeTile(0, V4(1,1,1,1), 0);
    Tile dirtTile = MakeTile(1, V4(1,1,1,1), 0);
    Tile stonePathTile = MakeTile(2, V4(1,1,1,1), 0);
    Tile wallTile = MakeTile(3, V4(1,1,1,1), TileFlags_Blocked | TileFlags_Opaque);
    Tile apronTile = MakeTile(3, V4(0,0,1,0.5f), TileFlags_Apron);
    
    for (int y = 0; y < level->h; y++)
    {
        for (int x = 0; x < level->w; x++)
        {
            Tile *tile;
            
            b32 isApron = (y == 0 || x == 0 || y == level->h - 1 || x == level->w - 1);
            if (isApron)
            {
                tile = &apronTile;
            }
            else if (RandomChance(0.5f))
            {
                tile = &grassTile;
            }
            else
            {
                tile = &dirtTile;
            }
            
            SetTile(level, x, y, *tile);
        }
    }
        
    
    const int roomsMax = 50;
    const int sizeMin = 6;
    const int sizeMax = 20;

    // 1 for apron + 1 wall thickness + 1 for an extra open tile before end of level
    const int roomCanvasPadding = 3;

    Room *rooms = MemoryArenaPushArray(level->arena, roomsMax, Room);

    int roomCount = 0;
    for (int i = 0; i < roomsMax; i++)
    {
        Room room;
        room.w = GetRandomValue(sizeMin, sizeMax);
        room.h = GetRandomValue(sizeMin, sizeMax);
        room.x = GetRandomValue(0 + roomCanvasPadding, level->w - room.w - roomCanvasPadding);
        room.y = GetRandomValue(0 + roomCanvasPadding, level->h - room.h - roomCanvasPadding);

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
    *whereToPlaceChest = V2(rooms[0].x + 1.5f, rooms[0].y + 1.5f);
    *whereToPlaceCampfire = V2(rooms[0].x + rooms[0].w - 1.5f, rooms[0].y + 1.5f);
}

api_func void GenerateLevel(Level *level, EntityStore *entityStore, LevelGenType genType, b32 generateEntities)
{
    v2 playerPos = {};
    v2 chestPos = {};
    v2 campfirePos = {};
    switch (genType)
    {
        case LevelGenType_Empty:
        {
            generateLevelEmpty(level, &playerPos);
        } break;
        
        case LevelGenType_OneRoom:
        {
            generateLevelOneRoom(level, &playerPos);
        } break;

        case LevelGenType_ClassicRooms:
        {
            generateLevelClassicRooms(level, &playerPos, &chestPos, &campfirePos);
        } break;

        default: InvalidCodePath;
    }

    if (generateEntities)
    {
        // TODO: Maybe don't do this inside level gen
        ActorStats stats = {};
        stats.viewRadius = 30;
        stats.attackReach = 1.0f;
        stats.combatRadius = 2.0f;
        stats.speed = 10.0f;
        stats.health = 100.0f;
        stats.maxHealth = 100.0f;
        Entity *player = AddActorEntity(entityStore,
                                        playerPos.x, playerPos.y, level,
                                        MakeSprite(SpriteAtlasName_Chars, 0),
                                        MakeCountedString("Player"));
        ConfigureActorEntity(player, stats);
        entityStore->controlledEntity = player;

        Entity *enemy = AddActorEntity(entityStore,
                                       30.0f, 30.0f, level,
                                       MakeSprite(SpriteAtlasName_Chars, 1),
                                       MakeCountedString("Enemy"));
        ConfigureActorEntity(enemy, stats);

        Entity chestBlueprint = MakeEntity(EntityType_Container,
                                           chestPos.x, chestPos.y, level,
                                           MakeSprite(SpriteAtlasName_World, 16), V4(1,1,1,1),
                                           MakeCountedString("Chest"),
                                           true, false);
        AddEntity(entityStore, chestBlueprint);

        Entity campfireBlueprint = MakeEntity(EntityType_Machine,
                                              campfirePos.x, campfirePos.y, level,
                                              MakeSprite(SpriteAtlasName_World, 17), V4(1,1,1,1),
                                              MakeCountedString("Campfire"),
                                              false, false);
        campfireBlueprint.machineData.machineType = MachineType_Campfire;
        AddEntity(entityStore, campfireBlueprint);
    }
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
    Assert(x >= 0 && x < level->w && y >= 0 && y < level->h);
    int i = XYToIdx(level->w, x, y);
    level->tileFlags[i] = tile.flags;
    SetTileSprite(&level->levelTilemap, x, y, tile.sprite);
}

api_func b32 IsTileBlocked(Level *level, int x, int y)
{
    Assert(x >= 0 && x < level->w && y >= 0 && y < level->h);
    int i = XYToIdx(level->w, x, y);
    b32 result = CheckFlag(level->tileFlags[i], TileFlags_Blocked);
    return result;
}

api_func b32 IsTileOpaque(Level *level, int x, int y)
{
    Assert(x >= 0 && x < level->w && y >= 0 && y < level->h);
    int i = XYToIdx(level->w, x, y);
    b32 result = CheckFlag(level->tileFlags[i], TileFlags_Opaque);
    return result;
}

api_func Direction4 IsTileApron(Level *level, int x, int y)
{
    Assert(x >= 0 && x < level->w && y >= 0 && y < level->h);
    int i = XYToIdx(level->w, x, y);
    b32 isApron = CheckFlag(level->tileFlags[i], TileFlags_Apron);

    Direction4 result;

    // TODO: This doesn't really work with Apron sizes larger than 1, which goes against having a special tile flag
    if (isApron)
    {
        if (y == 0)
        {
            result = Direction4_North;
        }
        else if (x == level->w - 1)
        {
            result = Direction4_East;
        }
        else if (y == level->h - 1)
        {
            result = Direction4_South;
        }
        else if (x == 0)
        {
            result = Direction4_West;
        }
        else
        {
            result = Direction4_None;
        }
    }
    else
    {
        result = Direction4_None;
    }
    
    return result;
}
