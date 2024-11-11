#include "sprites.h"

#include "sav.h"

global_var SpriteAtlasStore *_spriteAtlasStore;

#define LOAD_ATLAS(INDEX, PATH, HCOUNT, VCOUNT) _spriteAtlasStore->atlases[INDEX] = SavLoadTextureAtlas(PATH, HCOUNT, VCOUNT)

api_func void InitializeSprites(SpriteAtlasStore *placeForStore, MemoryArena *arena)
{
    _spriteAtlasStore = placeForStore;
    _spriteAtlasStore->atlasCount = SPRITE_ATLAS_COUNT;
    _spriteAtlasStore->atlases = MemoryArenaPushArrayAndZero(arena, _spriteAtlasStore->atlasCount, SavTextureAtlas);
    
    LOAD_ATLAS(SPRITE_ATLAS_ITEMS, "res/textures/monde_atlas_items.png", 16, 16);
    LOAD_ATLAS(SPRITE_ATLAS_CHARS, "res/textures/monde_atlas_chars.png", 16, 16);
}

api_func SavTextureAtlas GetAtlasForSprite(Sprite sprite)
{
    Assert(sprite.atlasType > SPRITE_ATLAS_NONE && sprite.atlasType < SPRITE_ATLAS_COUNT);
    SavTextureAtlas atlas = _spriteAtlasStore->atlases[sprite.atlasType];
    return atlas;
}

api_func SavTextureAtlas GetAtlasByType(SpriteAtlasType type)
{
    Assert(type > SPRITE_ATLAS_NONE && type < SPRITE_ATLAS_COUNT);
    SavTextureAtlas atlas = _spriteAtlasStore->atlases[type];
    return atlas;
}

api_func Sprite MakeSprite(SpriteAtlasType atlasType, int atlasValue)
{
    Sprite sprite;
    sprite.atlasType = atlasType;
    sprite.atlasValue = atlasValue;
    return sprite;
}
