#ifndef SPRITES_H
#define SPRITES_H

#include "sav.h"

enum SpriteAtlasName
{
    SpriteAtlasName_None = 0,
    
    SpriteAtlasName_World,
    SpriteAtlasName_Items,
    SpriteAtlasName_Chars,

    SpriteAtlasName_Count
};
global_var char *SpriteAtlasNameString[] = {"None", "World", "Items", "Chars"};

struct SpriteAtlasStore
{
    SavTextureAtlas *atlases;
    int atlasCount;
};

struct Sprite
{
    SpriteAtlasName atlasName;
    int atlasValue;
};

api_func void InitializeSprites(SpriteAtlasStore *placeForStore, MemoryArena *arena);
api_func SavTextureAtlas GetAtlasForSprite(Sprite sprite);
api_func SavTextureAtlas GetAtlasByName(SpriteAtlasName name);

api_func Sprite MakeSprite(SpriteAtlasName atlasName, int atlasValue);

#endif
