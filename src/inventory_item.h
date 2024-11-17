#ifndef INVENTORY_ITEM_H
#define INVENTORY_ITEM_H

#include "sav.h"
#include "sprites.h"
#include "inventory_item_spec.h"

struct InventoryItem
{
    int id;
    InventoryItemSpec *spec;
    f32 condition;
};

api_func InventoryItem InstantiateInventoryItemFromSpec(InventoryItemSpecType type);

#endif
