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
    Tile floorTile = MakeTile('.', SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT, 0);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(level, i, floorTile);
    }

    *whereToPlacePlayer = V2(20.0f, 20.0f);
}

internal_func void generateLevelOneRoom(Level *level, v2 *whereToPlacePlayer)
{
    Tile floorTile = MakeTile('.', SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT, 0);
    Tile wallTile = MakeTile('#', SAV_COLOR_SABLE, SAV_COLOR_OIL, TILE_BLOCKED | TILE_OPAQUE);
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
    Tile floorTile = MakeTile('.', SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT, 0);
    Tile wallTile = MakeTile('#', SAV_COLOR_SABLE, SAV_COLOR_OIL, TILE_BLOCKED | TILE_OPAQUE);
    for (int i = 0; i < level->w * level->h; i++)
    {
        SetTile(level, i, wallTile);
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
                    SetTile(level, x, y, floorTile);
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
                SetTile(level, x, constY, floorTile);
            }
            for (int y = leftCenter.y; y <= rightCenter.y; y++)
            {
                SetTile(level, constX, y, floorTile);
            }
        }
        else
        {
            for (int x = leftCenter.x; x <= rightCenter.x; x++)
            {
                SetTile(level, x, constY, floorTile);
            }
            for (int y = rightCenter.y; y <= leftCenter.y; y++)
            {
                SetTile(level, constX, y, floorTile);
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
    CharacterStats stats = {};
    stats.viewRadius = 10.0f;
    stats.attackReach = 1.0f;
    stats.combatRadius = 2.0f;
    stats.speed = 10.0f;
    stats.initiative = 10.0f;
    Entity playerEntity = MakeEntity(playerPos.x, playerPos.y, level, '@', SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY);
    ConfigureCharacterEntity(&playerEntity, stats);
    Entity *addedEntity = AddEntity(entityStore, playerEntity);
    entityStore->controlledEntity = addedEntity;

    stats.initiative = 15.0f;
    Entity enemyEntity = MakeEntity(30.0f, 30.0f, level, 'E', SAV_COLOR_SABLE, SAV_COLOR_RED);
    ConfigureCharacterEntity(&enemyEntity, stats);
    // AddEntity(entityStore, enemyEntity);
}

api_func void DrawLevel(Level *level)
{
    DrawTilemap(&level->levelTilemap, V2(0.0f, 0.0f));
}

api_func Tile MakeTile(i32 atlasValue, SavColor bg, SavColor fg, u8 flags)
{
    Tile tile;
    tile.sprite = MakeTileSprite(atlasValue, bg, fg);
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

enum EdgeDirection
{
    EDGE_NORTH,
    EDGE_EAST,
    EDGE_SOUTH,
    EDGE_WEST
};

api_func void PreprocessLevel(Level *level)
{
    LevelMesh *mesh = &level->mesh;

    if (mesh->arena.base == NULL)
    {
        mesh->arena = AllocArena(LEVEL_MESH_ARENA_SIZE);
    }
    MemoryArenaReset(&mesh->arena);
    mesh->w = level->w;
    mesh->h = level->h;
    mesh->vertCount = 0;
    mesh->edgeCount = 0;
    mesh->tiles = MemoryArenaPushArrayAndZero(&mesh->arena, mesh->w * mesh->h, MeshTile);
    mesh->verts = MemoryArenaPushArrayAndZero(&mesh->arena, LEVEL_MESH_MAX_VERTS, MeshVert);
    mesh->edges = MemoryArenaPushArrayAndZero(&mesh->arena, LEVEL_MESH_MAX_EDGES, MeshEdge);

    mesh->edgeTiles = MemoryArenaPushArrayAndZero(&mesh->arena, mesh->w * mesh->h, u8);

    for (int y = 1; y < level->h - 1; y++)
    {
        for (int x = 1; x < level->w - 1; x++)
        {
            if (IsTileOpaque(level, x, y))
            {
                if (!IsTileOpaque(level, x    , y - 1) ||
                    !IsTileOpaque(level, x + 1, y - 1) ||
                    !IsTileOpaque(level, x + 1, y    ) ||
                    !IsTileOpaque(level, x + 1, y + 1) ||
                    !IsTileOpaque(level, x    , y + 1) ||
                    !IsTileOpaque(level, x - 1, y + 1) ||
                    !IsTileOpaque(level, x - 1, y    ) ||
                    !IsTileOpaque(level, x - 1, y - 1))
                {
                    mesh->edgeTiles[XYToIdx(level->w, x, y)] = 1;
                }
            }
        }
    }

    for (int y = 1; y < level->h - 1; y++)
    {
        for (int x = 1; x < level->w - 1; x++)
        {
            int currentI = XYToIdx(level->w, x, y);
            int eastI = XYToIdx(level->w, x + 1, y);
            int southI = XYToIdx(level->w, x, y + 1);
            b32 isCurrentFilled = mesh->edgeTiles[currentI];
            b32 isEastFilled = mesh->edgeTiles[eastI];
            b32 isSouthFilled = mesh->edgeTiles[southI];
            b32 isHorizontalTransitionFromFilled = (isCurrentFilled && !isEastFilled);
            b32 isHorizontalTransitionFromOpen = (!isCurrentFilled && isEastFilled);
            b32 isHorizontalTransition = (isHorizontalTransitionFromFilled || isHorizontalTransitionFromOpen);
            b32 isVerticalTransitionFromFilled = (isCurrentFilled && !isSouthFilled);
            b32 isVerticalTransitionFromOpen = (!isCurrentFilled && isSouthFilled);
            b32 isVerticalTransition = (isVerticalTransitionFromFilled || isVerticalTransitionFromOpen);

            if (isHorizontalTransition)
            {
                MeshEdge *existingEdge = mesh->tiles[XYToIdx(level->w, x, y - 1)].edges[EDGE_EAST];
                if (existingEdge)
                {
                    mesh->tiles[currentI].edges[EDGE_EAST] = existingEdge;
                    existingEdge->verts[1]->p.y = (f32)(y + 1);
                }
                else
                {
                    MeshEdge *newEdge = mesh->edges + mesh->edgeCount++;
                    newEdge->verts[0] = mesh->verts + mesh->vertCount++;
                    newEdge->verts[1] = mesh->verts + mesh->vertCount++;
                    newEdge->verts[0]->p = V2((f32)(x + 1), (f32)y);
                    newEdge->verts[1]->p = V2((f32)(x + 1), (f32)(y + 1));
                    newEdge->n = isHorizontalTransitionFromFilled ? V2(1.0f, 0.0f) : V2(-1.0f, 0.0f);
                    mesh->tiles[currentI].edges[EDGE_EAST] = newEdge;
                }
            }

            if (isVerticalTransition)
            {
                MeshEdge *existingEdge = mesh->tiles[XYToIdx(level->w, x - 1, y)].edges[EDGE_SOUTH];
                if (existingEdge)
                {
                    mesh->tiles[currentI].edges[EDGE_SOUTH] = existingEdge;
                    existingEdge->verts[1]->p.x = (f32)(x + 1);
                }
                else
                {
                    MeshEdge *newEdge = mesh->edges + mesh->edgeCount++;
                    newEdge->verts[0] = mesh->verts + mesh->vertCount++;
                    newEdge->verts[1] = mesh->verts + mesh->vertCount++;
                    newEdge->verts[0]->p = V2((f32)(x), (f32)(y + 1));
                    newEdge->verts[1]->p = V2((f32)(x + 1), (f32)(y + 1));
                    newEdge->n = isVerticalTransitionFromFilled ? V2(0.0, 1.0f) : V2(0.0f, -1.0f);
                    mesh->tiles[currentI].edges[EDGE_SOUTH] = newEdge;
                }
            }
        }
    }
}

struct MapEdge { v2 a; v2 b; };

internal_func inline MapEdge getMapEdge(Level *level, int dir)
{
    switch (dir)
    {
        case EDGE_NORTH: return { V2(0.0f, 0.0f),                   V2((f32)level->w, 0.0f) };
        case EDGE_EAST:  return { V2((f32)level->w, 0.0f),          V2((f32)level->w, (f32)level->h) };
        case EDGE_SOUTH: return { V2((f32)level->w, (f32)level->h), V2(0.0f, (f32)level->h) };
        case EDGE_WEST:  return { V2(0.0f, (f32)level->h),          V2(0.0f, 0.0f) };
        default: InvalidCodePath; return {};
    }
}

struct ConnectingVertsResult
{
    v2 v[2];
    int count;
};

internal_func inline b32 areEdgesAdjacent(int edgeA, int edgeB)
{
    int d = edgeB - edgeA;
    return d == 1 || d == -3;
}

struct CastRayOnMapBoundaryResult
{
    int mapEdge;
    v2 projection;
};

internal_func CastRayOnMapBoundaryResult castRayOnMapBoundaries(Level *level, v2 p, v2 r)
{
    CastRayOnMapBoundaryResult result = {};

    b32 isFound = false;
    for (int mapEdgeI = 0; mapEdgeI < 4; mapEdgeI++)
    {
        MapEdge mapEdge = getMapEdge(level, mapEdgeI);

        v2 q = mapEdge.a;
        v2 s = mapEdge.b - mapEdge.a;

        f32 tDenom = VecCross2(r, s);
        f32 uDenom = VecCross2(s, r);

        if (AbsF32(tDenom) > CMP_EPSILON && AbsF32(uDenom) > CMP_EPSILON)
        {
            f32 t = VecCross2(q - p, s) / tDenom;
            f32 u = VecCross2(p - q, r) / uDenom;

            if (t >= 0.0f && u >= 0.0f && u <= 1.0f)
            {
                result.mapEdge = mapEdgeI;
                result.projection = p + t * r;
            
                isFound = true;
                break;
            }
        }
    }

    Assert(isFound);

    return result;
}

struct EdgeOcclusionPolygon
{
    b32 occlusionExists;
    v2 *verts;
    int vertCount;
};

internal_func EdgeOcclusionPolygon getEdgeOcclusionPolygon(Level *level, v2 pov, MeshEdge *edge, MemoryArena *resultArena)
{
    EdgeOcclusionPolygon result = {};

    // if (VecDot(pov - edge->verts[0]->p, edge->n) < -CMP_EPSILON)
    {
        // NOTE: pov is in negative half space of edge's line
        v2 x = edge->verts[0]->p;
        v2 y = edge->verts[1]->p;
        v2 px = x - pov;
        v2 py = y - pov;

        // NOTE: Keep p, x, y clockwise
        f32 signedArea = VecCross2(px, py);
        if (AbsF32(signedArea) <= CMP_EPSILON)
        {
            return result;
        }

        if (signedArea < 0.0f)
        {
            Swap(x, y, v2);
        }

        CastRayOnMapBoundaryResult xProj = castRayOnMapBoundaries(level, pov, x - pov);
        CastRayOnMapBoundaryResult yProj = castRayOnMapBoundaries(level, pov, y - pov);

        // NOTE: Up to 7 vertices are possible
        result.verts = MemoryArenaPushArrayAndZero(resultArena, 7, v2);
        int vertCount = 0;

        result.verts[result.vertCount++] = x;
        result.verts[result.vertCount++] = xProj.projection;

        int currentMapEdge = xProj.mapEdge;
        while (currentMapEdge != yProj.mapEdge)
        {
            result.verts[result.vertCount++] = getMapEdge(level, currentMapEdge).b;
        
            currentMapEdge++;
            if (currentMapEdge > 3)
            {
                currentMapEdge = 0;
            }
        }

        result.verts[result.vertCount++] = yProj.projection;
        result.verts[result.vertCount++] = y;
        MemoryArenaResizePreviousPushArray(resultArena, result.vertCount, v2);

        result.occlusionExists = true;
    }
        
    return result;
}

api_func void DrawLevelOcclusion(Level *level, v2 pov)
{
    for (int i = 0; i < level->mesh.edgeCount; i++)
    {
        EdgeOcclusionPolygon occlusion = getEdgeOcclusionPolygon(level, pov, &level->mesh.edges[i], MemoryArenaScratch(NULL));
        if (occlusion.occlusionExists)
        {
            for (int vertI = 0; vertI < occlusion.vertCount; vertI++)
            {
                occlusion.verts[vertI] *= level->levelTilemap.tilePxW;
            }
            
            DrawFilledPolygon(occlusion.verts, occlusion.vertCount, SAV_COLOR_PURPLE);
        }
    }
}

api_func void DebugDrawEdgeOcclusion(Level *level, v2 pov, int i)
{
    if (level->mesh.edgeCount > 0)
    {
        MeshEdge *edge = &level->mesh.edges[i % level->mesh.edgeCount];


            EdgeOcclusionPolygon occlusion = getEdgeOcclusionPolygon(level, pov, edge, MemoryArenaScratch(NULL));

            if (occlusion.occlusionExists)
            {
                for (int vertI = 0; vertI < occlusion.vertCount; vertI++)
                {
                    occlusion.verts[vertI] *= level->levelTilemap.tilePxW;
                }
            
                DrawFilledPolygon(occlusion.verts, occlusion.vertCount, SAV_COLOR_GRAY);
            
                for (int vertI = 0; vertI < occlusion.vertCount - 1; vertI++)
                {
                    SavColor c = (vertI == 0 ? SAV_COLOR_RED : SAV_COLOR_BLUE);
                    v2 p1 = occlusion.verts[vertI];
                    v2 p2 = occlusion.verts[vertI + 1];
                    DDrawLine(p1, p2, c);
                    DDrawPoint(p1, c);
                }

                DDrawPoint(occlusion.verts[occlusion.vertCount - 1] * level->levelTilemap.tilePxW, SAV_COLOR_BLUE);
            }
            else
            {
                v2 start = edge->verts[0]->p * level->levelTilemap.tilePxW;
                v2 end = edge->verts[1]->p * level->levelTilemap.tilePxW;
                DDrawLine(start, end, SAV_COLOR_PURPLE);
            }
    }
}

api_func void DebugDrawLevelMesh(Level *level)
{
    for (int i = 0; i < level->mesh.edgeCount; i++)
    {
        MeshEdge *edge = level->mesh.edges + i;
        v2 start = edge->verts[0]->p * level->levelTilemap.tilePxW;
        v2 end = edge->verts[1]->p * level->levelTilemap.tilePxW;

        v2 middle = (end + start) * 0.5f;
        v2 normalEnd = middle + edge->n * 0.5f * level->levelTilemap.tilePxW;

        DDrawLine(start, end, SAV_COLOR_YELLOW);
        DDrawPoint(start, SAV_COLOR_ORANGE);
        DDrawPoint(end, SAV_COLOR_ORANGE);
        DDrawLine(middle, normalEnd, SAV_COLOR_CYAN);
    }
}

api_func void DebugDrawEdgeTiles(Level *level)
{
    MemoryArena *scratch = MemoryArenaScratch(NULL);
    
    SavColor *colorsFg = MemoryArenaPushArray(scratch, level->w * level->h, SavColor);
    SavColor *colorsBg = MemoryArenaPushArray(scratch, level->w * level->h, SavColor);
    i32 *glyphs = MemoryArenaPushArray(scratch, level->w * level->h, i32);

    for (int i = 0; i < level->w * level->h; i++)
    {
        if (level->mesh.edgeTiles[i])
        {
            glyphs[i] =  'O';
            colorsFg[i] = SAV_COLOR_VIOLET;
            colorsBg[i] = SAV_COLOR_SABLE;
        }
        // else
        // {
        //     glyphs[i] =  '.';
        //     colorsFg[i] = SAV_COLOR_MIDNIGHT;
        //     colorsBg[i] = SAV_COLOR_SABLE;
        // }
    }
    
    DrawAtlasTilemap(*level->levelTilemap.atlas,
                     level->w, level->h,
                     level->levelTilemap.tilePxW,
                     level->levelTilemap.tilePxH,
                     glyphs, colorsFg, colorsBg,
                     V2(0.0f, 0.0f),
                     scratch);
}
