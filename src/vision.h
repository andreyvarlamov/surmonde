#ifndef VISION_H
#define VISION_H

#include "sav.h"

struct Level;

api_func b32 IsInLineOfSight(Level *level, v2i from, v2i target, i32 rangeLimit);
api_func void CalculateVision(Level *level, v2i p, i32 rangeLimit, u8 *visibleTiles);

#endif
