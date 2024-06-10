#include "sav.h"
#include "level.h"
#include "entity.h"
#include "turn_queue.h"

enum RunState
{
    RS_MAIN_MENU,
    RS_GAME_RUNNING
};

enum TurnState
{
    TS_WAITING,
    TS_PROCESSING
};

struct GameState
{
    MemoryArena worldArena;
    SavTextureAtlas mainTileAtlas;
    Level level;
    Camera2D camera;
    EntityStore entityStore;
    TurnQueue turnQueue;

    RunState runState;
    TurnState turnState;

    i64 currentTime;
};