#ifndef SPRITES_H
#define SPRITES_H

#include "sav.h"

enum SpriteAtlasType
{
    SPRITE_ATLAS_NONE = 0,
    
    SPRITE_ATLAS_ITEMS,
    SPRITE_ATLAS_CHARS,

    SPRITE_ATLAS_COUNT
};

struct SpriteAtlasStore
{
    SavTextureAtlas *atlases;
    int atlasCount;
};

struct Sprite
{
    SpriteAtlasType atlasType;
    int atlasValue;
};

api_func void InitializeSprites(SpriteAtlasStore *placeForStore, MemoryArena *arena);
api_func SavTextureAtlas GetAtlasForSprite(Sprite sprite);
api_func SavTextureAtlas GetAtlasByType(SpriteAtlasType type);

api_func Sprite MakeSprite(SpriteAtlasType atlasType, int atlasValue);

#endif
