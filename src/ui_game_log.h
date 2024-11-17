#ifndef UI_GAME_LOG_H
#define UI_GAME_LOG_H

struct GameLogEntry
{
    char string[GAME_LOG_ENTRY_MAX_CHARS];
};

struct GameLogState
{
    int entryCount;
    GameLogEntry entries[GAME_LOG_MAX_ENTRIES];
};

api_func void InitializeGameLogState(GameLogState *gameLogState);
api_func void AddGameLogEntry(char *entry, ...);
api_func void DrawGameLog();

#endif
