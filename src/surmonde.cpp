#include "surmonde.h"
#include <cstdio>
#include <sdl2/SDL_scancode.h>
#include <sdl2/SDL_mouse.h>
#include "sav.h"
#include "level.h"
#include "settings.h"
#include "helpers.h"

int main(int argc, char **argv)
{
    const char *windowName = "SURMONDE";
    InitWindow(windowName, 1920, 1080);

    GameMemory gameMemory = AllocGameMemory(Megabytes(4));
    GameState *gameState = (GameState *) gameMemory.data;

    SetTargetFPS(60.0);

    gameState->mainTileAtlas = SavLoadTextureAtlas("res/textures/hack64.png", 16, 16);
    SavSetTextureFilterMode(gameState->mainTileAtlas.texture, SAV_LINEAR);

    gameState->worldArena = AllocArena(Megabytes(4));

    gameState->level = MakeLevel(&gameState->worldArena, LEVEL_WIDTH, LEVEL_HEIGHT, &gameState->mainTileAtlas, LEVEL_ATLAS_SCALE);
    GenerateLevel(&gameState->level, LEVEL_ONE_ROOM);

    gameState->camera = MakeCamera(
        0.0f,
        GetWindowSize() / 2.0f,
        GetPxPFromTileP(GetLevelTilePxDim(&gameState->level), V2I(10, 10)),
        0.2f,
        5.0f,
        5);

    while (!WindowShouldClose())
    {
        BeginFrameTiming();

        PollEvents();

        if (MouseWheel() != 0) CameraIncreaseLogZoomSteps(&gameState->camera, MouseWheel());
        if (MouseDown(SDL_BUTTON_MIDDLE)) gameState->camera.target -= CameraScreenToWorldRel(&gameState->camera, MouseRelPos());

        SetWindowTitle(TextFormat("%s [%.2f fps]", windowName, GetFPSAvg()));

        UpdateLevel(&gameState->level);

        BeginDraw();
            ClearBackground(SAV_COLOR_LIGHTBLUE);

            BeginCameraMode(&gameState->camera);
                DrawLevel(&gameState->level);
            EndCameraMode();
        EndDraw();

        SavSwapBuffers();

        EndFrameTiming();
    }

    Quit();

    return 0;
}

#include "level.cpp"
#include "draw.cpp"