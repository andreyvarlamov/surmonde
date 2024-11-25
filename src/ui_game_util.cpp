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

    local_persist b32 encoded = false;
    local_persist char inputBuf[128] = {};
    local_persist u8 *encodedData = NULL;
    if (ImGui::InputText("Base64 Input", inputBuf, 128))
    {
        TraceLog("Base64 Input changed");
    }
    if (ImGui::Button("Base64 Encode"))
    {
        SavFree((void **)&encodedData);
        size_t charCount = 0; char *inputI = inputBuf; while (*inputI++) charCount++;
        size_t encodedSize = 0;
        encodedData = SavBase64Encode((u8 *)&inputBuf[0], charCount, &encodedSize);
    }

    if (ImGui::Button("Base64 Decode"))
    {
        // SavBase64Decode();
    }

    if (encodedData != NULL)
    {
        ImGui::Text("Base64 Result: %s", encodedData);
    }
    
    ImGui::End();
}
