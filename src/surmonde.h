#include "sav.h"
#include "level.h"
#include "entity.h"
#include "inventory.h"
#include "sprites.h"
#include "inventory_item_spec.h"

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
    
    b32 isPaused;

    RunState runState;
};
