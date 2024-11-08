#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "sav.h"

struct Level;
struct EntityStore;

struct NavPath
{
    b32 found;
    b32 alreadyAtTarget;
    
    v2 *path;
    int nodeCount;
};

api_func NavPath NavPathToTarget(Level *level, EntityStore *entityStore, v2 startF, v2 endF, MemoryArena *arena);

#endif
