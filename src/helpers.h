#ifndef HELPERS_H
#define HELPERS_H

#include "sav.h"

inline i32 XYToIdx(i32 w, i32 x, i32 y)
{
    return (x + y * w);
}

inline i32 VecToIdx(i32 w, v2i v)
{
    return (v.x + v.y * w);
}

inline v2i IdxToXY(i32 w, i32 i)
{
    return V2I(i % w, i / w);
}

inline v2i GetTilePFromFloatP(v2 floatP)
{
    v2i tileP = V2I((i32)floatP.x, (i32)floatP.y);
    return tileP;
}

inline v2 GetFloatPFromTileP(v2i tileP)
{
    v2 floatP = V2(tileP.x + 0.5f, tileP.y + 0.5f);
    return floatP;
}

inline b32 IsInRange(v2 from, v2 to, f32 range)
{
    return VecLengthSq(to - from) <= Square(range);
}

inline b32 AdvanceTimer(f32 *timer, f32 delta)
{
    *timer -= delta;
    if (*timer <= 0.0f)
    {
        *timer = 0.0f;
        return true;
    }
    return false;
}

struct IngameTimer
{
    f32 timer;
    b32 isRunning;
};

inline b32 AdvanceTimer(IngameTimer *timer, f32 length, f32 delta)
{
    if (!timer->isRunning)
    {
        timer->timer = length;
        timer->isRunning = true;
    }
    
    timer->timer -= delta;
    if (timer->timer <= 0.0f)
    {
        timer->timer = 0.0f;
        return true;
    }
    return false;
}

inline void ResetTimer(IngameTimer *timer)
{
    timer->isRunning = false;
}

inline b32 CheckFlag(u32 flags, u32 checkAgainst)
{
    b32 result = flags & checkAgainst;
    return result;
}

// TODO: Should this be here?

enum Direction4
{
    Direction4_None,
    Direction4_North,
    Direction4_East,
    Direction4_South,
    Direction4_West
};
const char *Direction4_Strings[] = {"None", "North", "East", "South", "West"};

enum Direction8
{
    Direction8_None,
    Direction8_North,
    Direction8_NorthEast,
    Direction8_East,
    Direction8_SouthEast,
    Direction8_South,
    Direction8_SouthWest,
    Direction8_West,
    Direction8_NorthWest
};
const char *Direction8_Strings[] = {"None", "North", "North-East", "East", "South-East", "South", "South-West", "West", "North-West"};

#endif
