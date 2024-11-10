#include "inventory_item.h"

#include "sav.h"

api_func InventoryItem InstantiateInventoryItemFromSpec(InventoryItemSpecType type)
{
    InventoryItem result;
    result.spec = GetInventoryItemSpecByType(type);
    result.condition = 1.0f;
    return result;
}
