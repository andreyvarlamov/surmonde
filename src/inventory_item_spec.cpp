#include "inventory_item_spec.h"

#include "sav.h"

global_var InventoryItemSpecStore *_inventoryItemSpecStore;

internal_func InventoryItemSpec makeInventoryItemSpec(SpriteAtlasType atlasType, int atlasIndex, char *name)
{
    InventoryItemSpec spec;
    spec.sprite = MakeSprite(atlasType, atlasIndex);
    Strcpy(spec.name, name);
    return spec;
}

#define DEFINE_ITEM_SPEC(TYPE, INDEX, NAME) _inventoryItemSpecStore->specs[TYPE] = makeInventoryItemSpec(SPRITE_ATLAS_ITEMS, INDEX, NAME)

api_func void InitializeInventoryItemSpecStore(InventoryItemSpecStore *placeForStore, MemoryArena *arena)
{
    _inventoryItemSpecStore = placeForStore;
    _inventoryItemSpecStore->specCount = ITEM_TYPE_COUNT;
    _inventoryItemSpecStore->specs = MemoryArenaPushArrayAndZero(arena, _inventoryItemSpecStore->specCount, InventoryItemSpec);

    DEFINE_ITEM_SPEC(ITEM_TYPE_RAW_STEAK, 0, "Raw Steak");
    DEFINE_ITEM_SPEC(ITEM_TYPE_BREAD_SLICE, 1, "Bread Slice");
    DEFINE_ITEM_SPEC(ITEM_TYPE_COOKED_STEAK, 2, "Cooked Steak");
    DEFINE_ITEM_SPEC(ITEM_TYPE_TOAST, 3, "Toast");
}

api_func InventoryItemSpec *GetInventoryItemSpecByType(InventoryItemSpecType type)
{
    Assert(type > ITEM_TYPE_NONE && type < ITEM_TYPE_COUNT);
    InventoryItemSpec *result = _inventoryItemSpecStore->specs + (int)type;
    return result;
}

