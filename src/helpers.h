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

inline 

#endif