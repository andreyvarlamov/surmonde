#ifndef INVENTORY_ITEM_H
#define INVENTORY_ITEM_H

#include "sav.h"
#include "sprites.h"
#include "inventory_item_spec.h"

struct InventoryItem
{
    InventoryItemSpec *spec;
    f32 condition;
};

api_func InventoryItem InstantiateInventoryItemFromSpec(InventoryItemSpecType type)
{
    InventoryItem result;
    result.spec = GetInventoryItemSpecByType(type);
    result.condition = 1.0f;
    return result;
}

#endif
