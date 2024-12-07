#ifndef ENTITY_MACHINE_H
#define ENTITY_MACHINE_H

#include "sav.h"
#include "inventory.h"

enum MachineType
{
    MachineType_None,
    MachineType_Campfire
};
global_var char *MachineTypeString[] = {"None", "Campfire"};

struct EntityDataMachine
{
    MachineType machineType;

    int processedItemIds[CAMPFIRE_ITEM_COUNT];
    f32 processedItemTimers[CAMPFIRE_ITEM_COUNT];
    int processedItemCount;
};

#define COOKING_DONE_TIME 5.0f

struct Entity;
void UpdateMachineEntity(Entity *e, f32 dT, InventoryStore *inventoryStore);

#endif
