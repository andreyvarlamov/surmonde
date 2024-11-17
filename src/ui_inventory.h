#ifndef UI_INVENTORY_H
#define UI_INVENTORY_H

#include "sav.h"
#include "entity.h"
#include "inventory.h"

struct ItemDragAndDropPayload
{
    Entity *sourceEntity;
    InventoryItem *item;
};

api_func void DrawInventoryUI(EntityStore *entityStore, InventoryStore *inventoryStore, Entity *e);

#endif
