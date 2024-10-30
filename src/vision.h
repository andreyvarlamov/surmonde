#ifndef VISION_H
#define VISION_H

#include "sav.h"

struct Level;

api_func void CalculateVision(Level *level, v2i p, i32 rangeLimit, u8 *visibleTiles);

#endif
