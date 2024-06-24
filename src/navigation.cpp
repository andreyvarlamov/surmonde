#include "navigation.h"

#include "sav.h"
#include "level.h"
#include "helpers.h"

struct NavSet
{
    int *data;
    int count;
};

internal_func void addToSet(NavSet *set, int value)
{
    Assert(set->count < NAVIGATION_MAX_OPEN_SET);
    set->data[set->count++] = value;
}

internal_func b32 isSetEmpty(NavSet *set)
{
    return set->count <= 0;
}

internal_func int getLeastFScoreNode(NavSet *set, i32 *fScores)
{
    int leastNode = {};
    i32 leastScore = INT_MAX;
    for (int i = 0; i < set->count; i++)
    {
        if (fScores[set->data[i]] < leastScore)
        {
            leastNode = set->data[i];
            leastScore = fScores[set->data[i]];
        }
    }
    return leastNode;
}

internal_func void removeFromSet(NavSet *set, int value)
{
    int toRemove = -1;
    for (int i = 0; i < set->count; i++)
    {
        if (set->data[i] == value)
        {
            toRemove = i;
            break;
        }
    }

    if (toRemove >= 0)
    {
        set->data[toRemove] = set->data[--set->count];
    }
}

internal_func b32 isInSet(NavSet *set, int value)
{
    for (int i = 0; i < set->count; i++)
    {
        if (set->data[i] == value)
        {
            return true;
        }
    }

    return false;
}

struct Neighbors
{
    int nodes[8];
    i32 costs[8];
    int count;
};

internal_func void addNeighbor(Neighbors *result, Level *level, int x, int y, i32 cost)
{
    if ((x >= 0 && x < level->w) &&
        (y >= 0 && y < level->h) &&
        !IsTileBlocked(level, x, y))
    {
        result->nodes[result->count] = XYToIdx(level->w, x, y);
        result->costs[result->count] = cost;
        result->count++;
    }
 }

internal_func Neighbors getNeighbors(Level *level, int nodeI)
{
    v2i node = IdxToXY(level->w, nodeI);
    
    Neighbors result = {};

    addNeighbor(&result, level, node.x - 1, node.y + 0, 10);
    addNeighbor(&result, level, node.x + 1, node.y + 0, 10);
    addNeighbor(&result, level, node.x + 0, node.y - 1, 10);
    addNeighbor(&result, level, node.x + 0, node.y + 1, 10);

    return result;
}

internal_func i32 getManhattanDist(v2i from, v2i to)
{
    i32 distX = AbsI32(from.x - to.x);
    i32 distY = AbsI32(from.y - to.y);
    i32 result = distX + distY;
    return result;
}

internal_func void reconstructPath(int fromI, int toI, Level *level, int *parents, NavPath *result, MemoryArena *arena, v2 toF)
{
    if (fromI != toI)
    {
        int pathNodeCount = 0;
        int currentNodeI = toI;
        while (currentNodeI != fromI)
        {
            currentNodeI = parents[currentNodeI];
            pathNodeCount++;
        }

        result->path = MemoryArenaPushArray(arena, pathNodeCount, v2);

        currentNodeI = toI;
        int resultIndex = pathNodeCount - 1;
        while (currentNodeI != fromI)
        {
            result->path[resultIndex--] = V2(IdxToXY(level->w, currentNodeI));
            currentNodeI = parents[currentNodeI];
        }

        Assert(resultIndex == -1);

        result->path[pathNodeCount - 1] = toF;

        result->nodeCount = pathNodeCount;
    }
    else
    {
        result->path = MemoryArenaPushArray(arena, 1, v2);
        result->path[0] = toF;
        result->nodeCount = 1;
    }
}

internal_func b32 lineOfSight(Level *level, int fromI, int toI)
{
    v2i from = IdxToXY(level->w, fromI); i32 x0 = from.x; i32 y0 = from.y;
    v2i to = IdxToXY(level->w, toI); i32 x1 = to.x; i32 y1 = to.y;

    i32 dx = AbsI32(x1 - x0);
    i32 dy = -AbsI32(y1 - y0);
    i32 e = dx + dy;

    i32 sx = -1; if (x0 < x1) sx = 1;
    i32 sy = -1; if (y0 < y1) sy = 1;

    while(true)
    {
        if (IsTileBlocked(level, x0, y0)) return false;
        if (x0 == x1 && y0 == y1) return true;
        i32 e2 = 2 * e;
        if (e2 >= dy) { if (x0 == x1) return true; e += dy; x0 += sx; }
        if (e2 <= dx) { if (y0 == y1) return true; e += dx; y0 += sy; }
    }
}

internal_func void updateNode(int nodeI, int neighborI, int endI, Level *level, int *parents, i32 *gScores, i32 *fScores, NavSet *openSet)
{
    v2i neighbor = IdxToXY(level->w, neighborI);
    v2i end = IdxToXY(level->w, endI);
    
    int parentToSet;
    i32 tentativeGScore;
    int parentI = parents[nodeI];
    if (parentI != -1 && lineOfSight(level, parentI, neighborI))
    {
        v2i parent = IdxToXY(level->w, parentI);
        tentativeGScore = gScores[nodeI] + getManhattanDist(parent, neighbor);
        parentToSet = parentI;
    }
    else
    {
        tentativeGScore = gScores[nodeI] + 10;
        parentToSet = nodeI;
    }

    if (tentativeGScore < gScores[neighborI])
    {
        parents[neighborI] = parentToSet;
        gScores[neighborI] = tentativeGScore;
        fScores[neighborI] = tentativeGScore + getManhattanDist(neighbor, end);

        if (!isInSet(openSet, neighborI))
        {
            addToSet(openSet, neighborI);
        }
    }
}

api_func NavPath NavPathToTarget(Level *level, v2 startF, v2 endF, MemoryArena *arena)
{
    if (VecEqual(startF, endF, CMP_EPSILON))
    {
        NavPath result = {};
        result.found = true;
        result.alreadyAtTarget = true;
        result.path = MemoryArenaPushArray(arena, 1, v2);
        result.path[0] = endF;
        return result;
    }
    
    v2i start = V2I(RoundF32ToI32(startF.x), RoundF32ToI32(startF.y));
    int startI = VecToIdx(level->w, start);
    v2i end = V2I(RoundF32ToI32(endF.x), RoundF32ToI32(endF.y));
    int endI = VecToIdx(level->w, end);

    NavSet openSet;
    openSet.data = MemoryArenaPushArray(arena, NAVIGATION_MAX_OPEN_SET, int);
    openSet.count = 0;

    int *parents = MemoryArenaPushArray(arena, level->w * level->h, int);
    i32 *gScores = MemoryArenaPushArray(arena, level->w * level->h, i32);
    i32 *fScores = MemoryArenaPushArray(arena, level->w * level->h, i32);
    b32 *visited = MemoryArenaPushArrayAndZero(arena, level->w * level->h, b32);
    
    for (int i = 0; i < level->w * level->h; i++)
    {
        gScores[i] = INT_MAX;
        fScores[i] = INT_MAX;
    }

    gScores[startI] = 0;
    fScores[startI] = getManhattanDist(start, end);
    parents[startI] = -1;
    
    addToSet(&openSet, startI);

    b32 foundPath = false;
    while (!isSetEmpty(&openSet))
    {
        int nodeI = getLeastFScoreNode(&openSet, fScores);
        if (nodeI == endI)
        {
            foundPath = true;
            break;
        }

        removeFromSet(&openSet, nodeI);
        visited[nodeI] = true;

        Neighbors neighbors = getNeighbors(level, nodeI);
        for (int n = 0; n < neighbors.count; n++)
        {
            int neighborI = neighbors.nodes[n];
            if (!visited[neighborI])
            {
                updateNode(nodeI, neighborI, endI, level, parents, gScores, fScores, &openSet);
            }
        }
    }

    NavPath result = {};
    result.found = foundPath;

    if (foundPath)
    {
        reconstructPath(startI, endI, level, parents, &result, arena, endF);
    }
    
    return result;
}

