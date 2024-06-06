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

inline v2
GetPxPFromTileP(v2 tilePxDim, v2i tileP)
{
    v2 pxP = V2(
        (f32) tileP.x * tilePxDim.x + tilePxDim.x / 2.0f,
        (f32) tileP.y * tilePxDim.y + tilePxDim.y / 2.0f
    );
    return pxP;
}
 
#endif