#include "sav.h"
#include "level.h"
#include "entity.h"
#include "inventory.h"
#include "sprites.h"
#include "inventory_item_spec.h"
#include "ui_game_log.h"

enum RunState
{
    RS_MAIN_MENU,
    RS_GAME_RUNNING
};

struct GameState
{
    MemoryArena worldArena;
    MemoryArena debugRenderArena;
    
    SavTextureAtlas worldAtlas;
    SavFont uiFont;

    SpriteAtlasStore spriteAtlasStore;
    InventoryItemSpecStore inventoryItemSpecStore;
    
    Level level;
    Camera2D camera;
    EntityStore entityStore;
    Entity *viewedEntities[VIEWED_ENTITY_MAX];

    InventoryStore inventoryStore;

    GameLogState gameLogState;

    Entity *entitiesWithOpenInventories[VIEWED_ENTITY_MAX];
    Entity *entitiesWithOpenMachineUi[VIEWED_ENTITY_MAX];
    
    b32 isPaused;

    RunState runState;
};

void AddEntityToSlot(Entity *e, Entity **slots, int slotCount);
