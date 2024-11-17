#include "ui_game_log.h"

#include "sav.h"

global_var GameLogState *_gameLogState;

api_func void InitializeGameLogState(GameLogState *gameLogState)
{
    _gameLogState = gameLogState;
}

api_func void AddGameLogEntry(char *entry, ...)
{
    va_list varArgs;
    va_start(varArgs, entry);
    StringFormat(entry, MakeStringBuffer(_gameLogState->entries[_gameLogState->entryCount++].string, GAME_LOG_ENTRY_MAX_CHARS), varArgs);
    va_end(varArgs);
}

api_func void DrawGameLog()
{
    ImGui::Begin("Game Log");

    local_persist int index = 0;
    local_persist b32 track = true;
    if (ImGui::Button("Add debug log"))
    {
        AddGameLogEntry("This is debug log #%d", index++);
    }
    ImGui::SameLine();
    // TODO: Enable and disable tracking automatically (need to know that it's being scrolled manually)
    ImGui::Checkbox("Track", (bool *)&track);

    ImGui::BeginChild("GameLogEntries");
    for (int i = 0; i < _gameLogState->entryCount; i++)
    {
        ImGui::Text("%s", _gameLogState->entries[i].string);
        if (track && i == _gameLogState->entryCount - 1)
        {
            ImGui::SetScrollHereY();
        }
    }
    ImGui::EndChild();

    ImGui::End();
}

