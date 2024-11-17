#include "entity_machine.h"

#include "sav.h"
#include "entity.h"
#include "inventory.h"

void updateCampfireEntity(Entity *e, f32 dT)
{
    e->machineData.timer += dT;
    TraceLog("%s processing timer: %.2f", e->name, e->machineData.timer);
}

void UpdateMachineEntity(Entity *e, f32 dT)
{
    Assert(e->type == EntityType_Machine);
    
    switch (e->machineData.machineType)
    {
        case MachineType_Campfire:
        {
            updateCampfireEntity(e, dT);
        } break;

        default: InvalidCodePath; // Unhandled MachineType
    }
}

