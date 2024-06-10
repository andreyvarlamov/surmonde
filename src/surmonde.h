#include "sav.h"
#include "level.h"
#include "entity.h"

enum RunState
{
    RS_MAIN_MENU,
    RS_GAME_RUNNING
};

struct GameState
{
    MemoryArena worldArena;
    SavTextureAtlas mainTileAtlas;
    Level level;
    Camera2D camera;
    EntityStore entityStore;

    RunState runState;
};