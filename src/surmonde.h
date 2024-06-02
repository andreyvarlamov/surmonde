#include "sav.h"
#include "level.h"

struct GameState
{
    MemoryArena worldArena;
    SavTextureAtlas mainTileAtlas;
    Level level;
};