#include "ui_game_util.h"

#include "sav.h"
#include "game_save.h"
#include "surmonde.h"

#include <imgui.h>

api_func void DrawGameUtilUI(GameState *gameState)
{
    ImGui::Begin("GameUtil");

    if (ImGui::Button("Save Game"))
    {
        SaveGame(gameState);
    }

    if (ImGui::Button("Load Game"))
    {
        LoadGame();
    }
    
    ImGui::End();
}
