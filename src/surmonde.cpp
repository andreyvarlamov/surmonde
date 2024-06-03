#include "surmonde.h"
#include <cstdio>
#include "sav.h"
#include "level.h"
#include "settings.h"

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

    while (!WindowShouldClose())
    {
        BeginFrameTiming();

        PollEvents();

        // RENDER
        SetWindowTitle(TextFormat("%s [%.2f fps]", windowName, GetFPSAvg()));

        BeginDraw();
            ClearBackground(SAV_COLOR_LIGHTBLUE);

            DrawLevel(&gameState->level);
        EndDraw();

        SavSwapBuffers();

        EndFrameTiming();
    }

    Quit();

    return 0;
}

#include "level.cpp"
#include "draw.cpp"