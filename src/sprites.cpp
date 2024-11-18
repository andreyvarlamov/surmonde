#include "sprites.h"

#include "sav.h"

global_var SpriteAtlasStore *_spriteAtlasStore;

#define LOAD_ATLAS(INDEX, PATH, HCOUNT, VCOUNT) _spriteAtlasStore->atlases[INDEX] = SavLoadTextureAtlas(PATH, HCOUNT, VCOUNT)

api_func void InitializeSprites(SpriteAtlasStore *placeForStore, MemoryArena *arena)
{
    _spriteAtlasStore = placeForStore;
    _spriteAtlasStore->atlasCount = SpriteAtlasName_Count;
    _spriteAtlasStore->atlases = MemoryArenaPushArrayAndZero(arena, _spriteAtlasStore->atlasCount, SavTextureAtlas);
    
    LOAD_ATLAS(SpriteAtlasName_World, "res/textures/monde_atlas.png", 16, 16);
    LOAD_ATLAS(SpriteAtlasName_Items, "res/textures/monde_atlas_items.png", 16, 16);
    LOAD_ATLAS(SpriteAtlasName_Chars, "res/textures/monde_atlas_chars.png", 16, 16);
}

api_func SavTextureAtlas GetAtlasForSprite(Sprite sprite)
{
    Assert(sprite.atlasName > SpriteAtlasName_None && sprite.atlasName < SpriteAtlasName_Count); // Sprite Atlas doesn't exist

    SavTextureAtlas atlas = _spriteAtlasStore->atlases[sprite.atlasName];

    Assert(atlas.texture.id > 0); // Sprite Atlas not initialized
    
    return atlas;
}

api_func SavTextureAtlas GetAtlasByName(SpriteAtlasName name)
{
    Assert(name > SpriteAtlasName_None && name < SpriteAtlasName_Count); // Sprite Atlas doesn't exist

    SavTextureAtlas atlas = _spriteAtlasStore->atlases[name];

    Assert(atlas.texture.id > 0); // Sprite Atlas not initialized
    
    return atlas;
}

api_func Sprite MakeSprite(SpriteAtlasName atlasName, int atlasValue)
{
    Sprite sprite;
    sprite.atlasName = atlasName;
    sprite.atlasValue = atlasValue;
    return sprite;
}
