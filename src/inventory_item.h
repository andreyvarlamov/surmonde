#ifndef INVENTORY_ITEM_H
#define INVENTORY_ITEM_H

#include "sav.h"
#include "sprite.h"

struct InventoryItem
{
    Sprite sprite;
    char name[ENTITY_NAME_CHARS];
};

#endif
