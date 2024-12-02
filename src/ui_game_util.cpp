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
    local_persist size_t encodedSize = 0;
    local_persist u8 *encodedData = NULL;
    local_persist size_t decodedSize = 0;
    local_persist u8 *decodedData = NULL;
    if (ImGui::InputText("Base64 Input", inputBuf, 128))
    {
        TraceLog("Base64 Input changed");
    }
    if (ImGui::Button("Base64 Encode"))
    {
        SavFree((void **)&encodedData);
        size_t charCount = 0; char *inputI = inputBuf; while (*inputI++) charCount++;
        encodedData = SavBase64Encode((u8 *)&inputBuf[0], charCount, &encodedSize);
    }

    if (ImGui::Button("Base64 Decode"))
    {
        SavFree((void **)&decodedData);
        decodedData = SavBase64Decode(encodedData, encodedSize, &decodedSize, true);
    }

    if (encodedData != NULL)
    {
        ImGui::InputText("Base64 Encoded", (char *)encodedData, encodedSize);
    }
    
    if (decodedData != NULL)
    {
        ImGui::InputText("Base64 Decoded", (char *)decodedData, decodedSize);
    }

    ImGui::End();
}
