#ifndef HELPERS_H
#define HELPERS_H

#include "sav.h"

inline i32 XYToIdx(i32 w, i32 x, i32 y)
{
    return (x + y * w);
}

inline v2i IdxToXY(i32 w, i32 i)
{
    return V2I(i % w, i / w);
}

inline v2 GetPxPFromTileP(v2 tilePxDim, v2i tileP)
{
    v2 pxP = V2(
        (f32) tileP.x * tilePxDim.x + tilePxDim.x / 2.0f,
        (f32) tileP.y * tilePxDim.y + tilePxDim.y / 2.0f
    );
    return pxP;
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

inline b32 RandomChance(f32 chance)
{
    b32 result = GetRandomFloat() <= chance * 0.01f;
    return result;
}
 
#endif
