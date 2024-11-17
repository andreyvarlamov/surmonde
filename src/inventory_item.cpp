#include "inventory_item.h"

#include "sav.h"

api_func InventoryItem InstantiateInventoryItemFromSpec(InventoryItemSpecType type, int *itemIdSeed)
{
    InventoryItem result;
    result.id = (*itemIdSeed)++;
    result.spec = GetInventoryItemSpecByType(type);
    result.condition = 1.0f;
    return result;
}
