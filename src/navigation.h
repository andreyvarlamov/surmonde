#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "sav.h"

struct Level;

struct NavPath
{
    b32 found;
    
    v2i *path;
    int nodeCount;
};

api_func NavPath NavPathToTarget(Level *level, v2 from, v2 to, MemoryArena *arena);

#endif
