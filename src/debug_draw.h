#ifndef DEBUG_DRAW_H
#define DEBUG_DRAW_H

#include "sav.h"

api_func void DDrawInit(MemoryArena *arena);

api_func void DDrawPoint(v2 point, SavColor color);
api_func void DDrawLine(v2 start, v2 end, SavColor color);

api_func void DDraw();

#endif
