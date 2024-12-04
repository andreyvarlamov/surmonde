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

    ImGui::Separator();

    ImGui::Text("Base64 Test");

    local_persist char inputBuf[128] = {};
    local_persist char encodedBuf[128] = {};
    local_persist char decodedBuf[128] = {};
    
    local_persist size_t encodedSize = 0;
    local_persist u8 *encodedData = NULL;

    local_persist size_t decodedSize = 0;
    local_persist u8 *decodedData = NULL;

    ImGui::InputText("Input", inputBuf, 128);
    if (ImGui::Button("Encode"))
    {
        SavFree((void **)&encodedData);
        size_t charCount = 0; char *inputI = inputBuf; while (*inputI++) charCount++;
        encodedData = SavBase64Encode((u8 *)&inputBuf[0], charCount, &encodedSize);
        Assert(encodedSize < 128);
        strcpy((char *)encodedBuf, (char *)encodedData);
    }

    ImGui::InputText("Encoded", encodedBuf, 128);
    
    if (ImGui::Button("Decode"))
    {
        SavFree((void **)&decodedData);
        size_t charCount = 0; char *inputI = encodedBuf; while (*inputI++) charCount++;
        decodedData = SavBase64Decode((u8 *)&encodedBuf[0], charCount, &decodedSize, true);
        Assert(decodedSize < 128);
        Assert(decodedData[decodedSize] == '\0');
        strcpy((char *)decodedBuf, (char *)decodedData);
    }

    ImGui::InputText("Decoded", decodedBuf, 128);

    ImGui::Separator();

    ImGui::End();
}
