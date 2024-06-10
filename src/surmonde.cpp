#include "surmonde.h"
#include <cstdio>
#include <sdl2/SDL_scancode.h>
#include <sdl2/SDL_mouse.h>
#include "sav.h"
#include "level.h"
#include "settings.h"
#include "helpers.h"
#include "turn_queue.h"

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

    f32 tilePxW = gameState->mainTileAtlas.cellW * LEVEL_ATLAS_SCALE;
    f32 tilePxH = gameState->mainTileAtlas.cellH * LEVEL_ATLAS_SCALE;
    gameState->level = MakeLevel(LEVEL_WIDTH, LEVEL_HEIGHT, &gameState->mainTileAtlas, tilePxW, tilePxH, &gameState->worldArena);
    gameState->entityStore = MakeEntityStore(&gameState->worldArena, ENTITY_STORE_COUNT, LEVEL_WIDTH, LEVEL_HEIGHT);
    // gameState->level.entityTilemap = MakeTilemap(arena, atlas, tilePxW, tilePxH, w, h);
    GenerateLevel(&gameState->level, &gameState->entityStore, LEVEL_ONE_ROOM);

    gameState->camera = MakeCamera(
        0.0f,
        GetWindowSize() / 2.0f,
        GetPxPFromTileP(GetLevelTilePxDim(&gameState->level), V2I(10, 10)),
        0.2f,
        5.0f,
        5);

    gameState->turnQueue = MakeTurnQueue(&gameState->worldArena);

    while (!WindowShouldClose())
    {
        BeginFrameTiming();

        PollEvents();

        if (MouseWheel() != 0) CameraIncreaseLogZoomSteps(&gameState->camera, MouseWheel());
        if (MouseDown(SDL_BUTTON_MIDDLE)) gameState->camera.target -= CameraScreenToWorldRel(&gameState->camera, MouseRelPos());

        SetWindowTitle(TextFormat("%s [%.2f fps]", windowName, GetFPSAvg()));

        switch (gameState->runState)
        {
            case RS_MAIN_MENU:
            {
                gameState->runState = RS_GAME_RUNNING;
            } break;

            case RS_GAME_RUNNING:
            {
                TurnQueueNode nextInQueue = TurnQueuePeek(&gameState->turnQueue);
                gameState->currentTime = nextInQueue.time;
                if (nextInQueue.t == TURN_QUEUE_ACTION && IsControlledEntity(&gameState->entityStore, nextInQueue.e))
                {
                    PlayerInput input = {}; // = ProcessPlayerInput();
                    // if (IsValidInput(input))
                    {
                        Assert(IsControlledEntity(&gameState->entityStore, nextInQueue.e));

                        TurnQueuePop(&gameState->turnQueue);

                        i64 actionCost = ProcessControlledEntity(&gameState->entityStore, input, nextInQueue.e);

                        TurnQueueInsertEntityAction(&gameState->turnQueue, nextInQueue.e, nextInQueue.time + actionCost);
                    }
                }

                int queueItemsProcessed = 0;
                while (nextInQueue.t == TURN_QUEUE_TURN_MARKER || !IsControlledEntity(&gameState->entityStore, nextInQueue.e))
                {
                    if (nextInQueue.t == TURN_QUEUE_ACTION)
                    {
                        TurnQueuePop(&gameState->turnQueue);

                        i64 actionCost = ProcessEntityTurn(&gameState->entityStore, nextInQueue.e);

                        TurnQueueInsertEntityAction(&gameState->turnQueue, nextInQueue.e, nextInQueue.time + actionCost);

                        nextInQueue = TurnQueuePeek(&gameState->turnQueue);
                        gameState->currentTime = nextInQueue.time;
                    }
                    else
                    {
                        TraceLog("Turn marker: %lld", nextInQueue.time);

                        // Do things that should happen every game turn
                    }

                    if (++queueItemsProcessed >= QUEUE_MAX_PROCESSED_PER_FRAME)
                    {
                        break;
                    }
                }

                BeginDraw();
                    ClearBackground(SAV_COLOR_LIGHTBLUE);

                    BeginCameraMode(&gameState->camera);
                        DrawLevel(&gameState->level);
                        // DrawTilemap(&level->entityTilemap);
                    EndCameraMode();
                EndDraw();
            } break;
        }

        SavSwapBuffers();

        EndFrameTiming();
    }

    Quit();

    return 0;
}

#include "level.cpp"
#include "draw.cpp"
#include "tilemap.cpp"
#include "entity.cpp"
#include "turn_queue.cpp"