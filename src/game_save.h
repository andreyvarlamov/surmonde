#ifndef GAME_SAVE_H
#define GAME_SAVE_H

#include "sav.h"

struct GameState;

api_func void SaveGame(GameState *gameState);
api_func void LoadGame();

#endif
