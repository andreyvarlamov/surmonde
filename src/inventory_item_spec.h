#ifndef INVENTORY_ITEM_SPEC_H
#define INVENTORY_ITEM_SPEC_H

#include "sav.h"

struct InventoryItemSpec
{
    Sprite sprite;
    char name[ENTITY_NAME_CHARS];
};

enum InventoryItemSpecType
{
    ITEM_TYPE_NONE = 0,

    ITEM_TYPE_RAW_STEAK,
    ITEM_TYPE_BREAD_SLICE,
    ITEM_TYPE_COOKED_STEAK,
    ITEM_TYPE_TOAST,
    
    ITEM_TYPE_COUNT
};

struct InventoryItemSpecStore
{
    InventoryItemSpec *specs;
    int specCount;
};

api_func void InitializeInventoryItemSpecStore(InventoryItemSpecStore *placeForStore, MemoryArena *arena);
api_func InventoryItemSpec *GetInventoryItemSpecByType(InventoryItemSpecType type);

#endif
