#ifndef ENTITY_MACHINE_H
#define ENTITY_MACHINE_H

#include "sav.h"

enum MachineType
{
    MachineType_None,
    MachineType_Campfire
};

struct EntityDataMachine
{
    MachineType machineType;
    f32 timer;
};

struct Entity;
void UpdateMachineEntity(Entity *e, f32 dT);

#endif
