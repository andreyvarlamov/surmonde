#ifndef SPRITE_H
#define SPRITE_H

#include "sav.h"

struct Sprite
{
    SavTextureAtlas *atlas;
    int atlasValue;
    v4 color;
};

#endif
