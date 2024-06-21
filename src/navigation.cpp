#include "navigation.h"

#include "sav.h"
#include "level.h"
#include "helpers.h"

struct NavSet
{
    v2i *data;
    int count;
};

internal_func void addToSet(NavSet *set, v2i value)
{
    set->data[set->count++] = value;
}

internal_func b32 isSetEmpty(NavSet *set)
{
    return set->count <= 0;
}

internal_func v2i getLeastFScoreNode(NavSet *set, int levelW, i32 *fScores)
{
    v2i leastNode = {};
    i32 leastScore = INT_MAX;
    for (int i = 0; i < set->count; i++)
    {
        int pI = VecToIdx(levelW, set->data[i]);
        if (fScores[pI] < leastScore)
        {
            leastNode = set->data[i];
            leastScore = fScores[pI];
        }
    }
    return leastNode;
}

internal_func void removeFromSet(NavSet *set, v2i value)
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

internal_func b32 isInSet(NavSet *set, v2i value)
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
    v2i nodes[8];
    i32 costs[8];
    int count;
};

internal_func void addNeighbor(Neighbors *result, Level *level, v2i node, i32 cost)
{
    if ((node.x >= 0 && node.x < level->w) &&
        (node.y >= 0 && node.y < level->h) &&
        !IsTileBlocked(level, node.x, node.y))
    {
        result->nodes[result->count] = node;
        result->costs[result->count] = cost;
        result->count++;
    }
 }

internal_func Neighbors getNeighbors(Level *level, v2i node)
{
    Neighbors result = {};

    addNeighbor(&result, level, V2I(node.x - 1, node.y + 0), 10);
    addNeighbor(&result, level, V2I(node.x + 1, node.y + 0), 10);
    addNeighbor(&result, level, V2I(node.x + 0, node.y - 1), 10);
    addNeighbor(&result, level, V2I(node.x + 0, node.y + 1), 10);

    return result;
}

internal_func i32 getManhattanDist(v2i from, v2i to)
{
    i32 distX = AbsI32(from.x - to.x);
    i32 distY = AbsI32(from.y - to.y);
    i32 result = distX + distY;
    return result;
}

#if 0
internal_func i32 getHCost(v2i from, v2i to)
{
    i32 distX = AbsI32(from.x - to.x);
    i32 distY = AbsI32(from.y - to.y);

    i32 minDist = Min(distX, distY);
    i32 maxDist = Max(distX, distY);

    i32 result = 14 * minDist + 10 * (maxDist - minDist);
    return result;
}
#endif

internal_func void reconstructPath(int fromI, int toI, Level *level, int *parents, NavPath *result, MemoryArena *arena)
{
    int pathNodeCount = 1;
    int currentNodeI = toI;
    while (currentNodeI != fromI)
    {
        currentNodeI = parents[currentNodeI];
        pathNodeCount++;
    }

    result->path = MemoryArenaPushArray(arena, pathNodeCount, v2i);

    currentNodeI = toI;
    int resultIndex = pathNodeCount - 1;
    while (currentNodeI != fromI)
    {
        result->path[resultIndex--] = IdxToXY(level->w, currentNodeI);
        currentNodeI = parents[currentNodeI];
        pathNodeCount++;
    }

    Assert(resultIndex == 0);

    result->path[resultIndex] = IdxToXY(level->w, currentNodeI);
    result->nodeCount = pathNodeCount;
}

internal_func b32 lineOfSight(Level *level, v2i from, v2i to)
{
    return true;
}

internal_func void updateNode(v2i node, v2i neighbor, v2i end, Level *level, int *parents, i32 *gScores, i32 *fScores, NavSet *openSet)
{
    int neighborI = VecToIdx(level->w, neighbor);
    int nodeI = VecToIdx(level->w, node);
    v2i parent = IdxToXY(level->w, parents[nodeI]);
    v2i parentToSet;
    i32 tentativeGScore;
    if (lineOfSight(level, parent, neighbor))
    {
        tentativeGScore = gScores[nodeI] + getManhattanDist(parent, neighbor);
        parentToSet = parent;
    }
    else
    {
        tentativeGScore = gScores[nodeI] + 10;
        parentToSet = node;
    }

    if (tentativeGScore < gScores[neighborI])
    {
        parents[neighborI] = VecToIdx(level->w, parentToSet);
        gScores[neighborI] = tentativeGScore;
        fScores[neighborI] = tentativeGScore + getManhattanDist(neighbor, end);

        if (!isInSet(openSet, neighbor))
        {
            addToSet(openSet, neighbor);
        }
    }
}

api_func NavPath NavPathToTarget(Level *level, v2 fromF, v2 toF, MemoryArena *arena)
{
    v2i from = V2I(RoundF32ToI32(fromF.x), RoundF32ToI32(fromF.y));
    v2i to = V2I(RoundF32ToI32(toF.x), RoundF32ToI32(toF.y));

    NavSet openSet;
    openSet.data = MemoryArenaPushArray(arena, NAVIGATION_MAX_OPEN_SET, v2i);
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

    int startI = VecToIdx(level->w, from);
    gScores[startI] = 0;
    fScores[startI] = getManhattanDist(from, to);
    
    addToSet(&openSet, from);

    b32 foundPath = false;
    while (!isSetEmpty(&openSet))
    {
        v2i node = getLeastFScoreNode(&openSet, level->w, fScores);
        if (node == to)
        {
            foundPath = true;
            break;
        }

        int nodeI = VecToIdx(level->w, node);

        removeFromSet(&openSet, node);

        visited[nodeI] = true;

        Neighbors neighbors = getNeighbors(level, node);
        for (int n = 0; n < neighbors.count; n++)
        {
            v2i neighbor = neighbors.nodes[n];

            if (!visited[VecToIdx(level->w, neighbor)])
            {
                updateNode(node, neighbor, to, level, parents, gScores, fScores, &openSet);
            }
        }
    }

    NavPath result;
    result.found = foundPath;

    if (foundPath)
    {
        reconstructPath(VecToIdx(level->w, from), VecToIdx(level->w, to), level, parents, &result, arena);
    }
    
    return result;
}

