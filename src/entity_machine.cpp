#include "entity_machine.h"

#include "sav.h"
#include "entity.h"
#include "inventory.h"
#include "ui_game_log.h"

internal_func void updateCampfireEntity(Entity *e, f32 dT, InventoryStore *inventoryStore)
{
    EntityDataMachine *m = &e->machineData;

    for (int i = 0; i < CAMPFIRE_ITEM_COUNT; i++)
    {
        int itemId = m->processedItemIds[i];
        if (itemId > 0)
        {
            InventoryItem *item = FindItemInInventoryById(e->inventory, itemId);
            if (item != NULL)
            {
                f32 oldTimer = m->processedItemTimers[i];
                m->processedItemTimers[i] += dT;
                if (m->processedItemTimers[i] >= COOKING_DONE_TIME && oldTimer < COOKING_DONE_TIME)
                {
                    char *oldName = item->spec->name;
                    if (item->spec->type == ItemType_RawSteak)
                    {
                        item->spec = GetInventoryItemSpecByType(ItemType_CookedSteak);
                    }
                    else if (item->spec->type == ItemType_BreadSlice)
                    {
                        item->spec = GetInventoryItemSpecByType(ItemType_Toast);
                    }
                    AddGameLogEntry("%s done processing item: %s -> %s", e->name, oldName, item->spec->name);
                }
            }
            else
            {
                m->processedItemIds[i] = 0;
            }
        }
    }

    InventoryItemIterator it = GetInventoryItemIterator(inventoryStore, e->inventory);
    for (InventoryItem *item = NextInventoryItem(&it);
         item != NULL;
         item = NextInventoryItem(&it))
    {
        int freeSlot = -1;
        b32 foundItem = false;
        for (int i = 0; i < CAMPFIRE_ITEM_COUNT; i++)
        {
            int itemId = m->processedItemIds[i];
            if (itemId == 0)
            {
                if (freeSlot < 0)
                {
                    freeSlot = i;
                }
            }
            else if (itemId == item->id)
            {
                foundItem = true;
            }
        }

        if (!foundItem)
        {
            Assert(freeSlot >= 0);
            m->processedItemIds[freeSlot] = item->id;
            m->processedItemTimers[freeSlot] = 0.0f;
        }
    }
}

void UpdateMachineEntity(Entity *e, f32 dT, InventoryStore *inventoryStore)
{
    Assert(e->type == EntityType_Machine);
    
    switch (e->machineData.machineType)
    {
        case MachineType_Campfire:
        {
            updateCampfireEntity(e, dT, inventoryStore);
        } break;

        default: InvalidCodePath; // Unhandled MachineType
    }
}

