#include "navigation.h"

#include "sav.h"
#include "level.h"
#include "helpers.h"

struct NavOpenSet
{
    v2i *data;
    int count;
};

internal_func void addToOpenSet(NavOpenSet *set, v2i value)
{
    set->data[set->count++] = value;
}

internal_func b32 isOpenSetEmpty(NavOpenSet *set)
{
    return set->count <= 0;
}

internal_func v2i getLeastFScoreNode(NavOpenSet *set, int levelW, i32 *fScores)
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
}

internal_func void removeFromOpenSet(NavOpenSet *set, v2i value)
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

internal_func b32 isInOpenSet(NavOpenSet *set, v2i value)
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

internal_func Neighbors getNeighbors(Level *level, v2i node)
{
    Neighbors result = {};
    for (int y = -1; y <= 1; y++)
    {
        for (int x = -1; x <= 1; x++)
        {
            if (!(y == 0 && x == 0) &&
                (node.x + x >= 0 && node.x + x < level->w) &&
                (node.y + y >= 0 && node.y + y < level->h))
            {
                result.nodes[result.count] = node;
                result.costs[result.count] = (x == 0 || y == 0) ? 10 : 14;
                result.count++;
            }
        }
    }

    return result;
}

internal_func i32 getHCost(v2i from, v2i to)
{
    i32 distX = AbsI32(from.x - to.x);
    i32 distY = AbsI32(from.y - to.y);

    i32 minDist = Min(distX, distY);
    i32 maxDist = Max(distX, distY);

    i32 result = 14 * minDist + 10 * (maxDist - minDist);
    return result;
}

api_func NavPath NavPathToTarget(Level *level, v2 fromF, v2 toF, MemoryArena *arena)
{
    v2i from = V2I(RoundF32ToI32(fromF.x), RoundF32ToI32(fromF.y));
    v2i to = V2I(RoundF32ToI32(toF.x), RoundF32ToI32(toF.y));

    NavOpenSet openSet;
    openSet.data = MemoryArenaPushArray(arena, NAVIGATION_MAX_OPEN_SET, v2i);
    openSet.count = 0;

    int *parents = MemoryArenaPushArray(arena, level->w * level->h, int);
    i32 *gScores = MemoryArenaPushArray(arena, level->w * level->h, i32);
    i32 *fScores = MemoryArenaPushArray(arena, level->w * level->h, i32);
    for (int i = 0; i < level->w * level->h; i++)
    {
        gScores[i] = INT_MAX;
        fScores[i] = INT_MAX;
    }

    int startI = VecToIdx(level->w, from);
    gScores[startI] = 0;
    fScores[startI] = getHCost(from, to);
    
    addToOpenSet(&openSet, from);

    b32 foundPath = false;
    while (!isOpenSetEmpty(&openSet))
    {
        v2i node = getLeastFScoreNode(&openSet, level->w, fScores);
        if (node == to)
        {
            foundPath = true;
            break;
        }

        int nodeI = VecToIdx(level->w, node);

        removeFromOpenSet(&openSet, node);

        Neighbors neighbors = getNeighbors(level, node);
        for (int n = 0; n < neighbors.count; n++)
        {
            v2i neighbor = neighbors.nodes[n];
            i32 neighborCost = neighbors.costs[n];
            int neighborI = VecToIdx(level->w, neighbor);

            i32 tentativeGScore = gScores[nodeI] + neighborCost;
            if (tentativeGScore < gScores[neighborI])
            {
                parents[neighborI] = nodeI;
                gScores[neighborI] = tentativeGScore;
                fScores[neighborI] = tentativeGScore + getHCost(neighbor, to);

                if (!isInOpenSet(&openSet, neighbor))
                {
                    addToOpenSet(&openSet, neighbor);
                }
            }
        }
    }

    NavPath result;
    result.foundPath = foundPath;
    #if 0
    if (foundPath)
    {
        result.path[result.path++] = ;
    }
    #endif
    
    return result;
}
