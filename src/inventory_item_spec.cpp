#include "inventory_item_spec.h"

#include "sav.h"

global_var InventoryItemSpecStore *_inventoryItemSpecStore;

internal_func InventoryItemSpec makeInventoryItemSpec(InventoryItemSpecType type, SpriteAtlasType atlasType, int atlasIndex, char *name)
{
    InventoryItemSpec spec;
    spec.type = type;
    spec.sprite = MakeSprite(atlasType, atlasIndex);
    Strcpy(spec.name, name);
    return spec;
}

#define DEFINE_ITEM_SPEC(TYPE, INDEX, NAME) _inventoryItemSpecStore->specs[TYPE] = makeInventoryItemSpec(TYPE, SPRITE_ATLAS_ITEMS, INDEX, NAME)

api_func void InitializeInventoryItemSpecStore(InventoryItemSpecStore *placeForStore, MemoryArena *arena)
{
    _inventoryItemSpecStore = placeForStore;
    _inventoryItemSpecStore->specCount = ItemType_Count;
    _inventoryItemSpecStore->specs = MemoryArenaPushArrayAndZero(arena, _inventoryItemSpecStore->specCount, InventoryItemSpec);

    DEFINE_ITEM_SPEC(ItemType_RawSteak, 0, "Raw Steak");
    DEFINE_ITEM_SPEC(ItemType_BreadSlice, 1, "Bread Slice");
    DEFINE_ITEM_SPEC(ItemType_CookedSteak, 2, "Cooked Steak");
    DEFINE_ITEM_SPEC(ItemType_Toast, 3, "Toast");
}

api_func InventoryItemSpec *GetInventoryItemSpecByType(InventoryItemSpecType type)
{
    Assert(type > ItemType_None && type < ItemType_Count);
    InventoryItemSpec *result = _inventoryItemSpecStore->specs + (int)type;
    return result;
}

