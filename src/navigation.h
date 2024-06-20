#ifndef NAVIGATION_H
#define NAVIGATION_H

#include "sav.h"

struct NavState
{
    int w, h;
    u8 *tileFlags;
    
    MemoryArena *arena;
};

struct NavPath
{
};

api_func NavState InitializeNavigation(int w, int h, u8 *tileFlags, MemoryArena *arena);
api_func NavPath NavPathToTarget(NavState *state, v2 from, v2 to);

#endif
