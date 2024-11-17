#ifndef INVENTORY_ITEM_SPEC_H
#define INVENTORY_ITEM_SPEC_H

#include "sav.h"

enum InventoryItemSpecType
{
    ItemType_None = 0,

    ItemType_RawSteak,
    ItemType_BreadSlice,
    ItemType_CookedSteak,
    ItemType_Toast,
    
    ItemType_Count
};

struct InventoryItemSpec
{
    InventoryItemSpecType type;
    Sprite sprite;
    char name[ENTITY_NAME_CHARS];
};

struct InventoryItemSpecStore
{
    InventoryItemSpec *specs;
    int specCount;
};

api_func void InitializeInventoryItemSpecStore(InventoryItemSpecStore *placeForStore, MemoryArena *arena);
api_func InventoryItemSpec *GetInventoryItemSpecByType(InventoryItemSpecType type);

#endif
