#include "surmonde.h"
#include <cstdio>
#include <sdl2/SDL_scancode.h>
#include <sdl2/SDL_mouse.h>
#include "sav.h"
#include "level.h"
#include "defines.h"
#include "helpers.h"
#include "draw.h"

int main(int argc, char **argv)
{
    const char *windowName = "SURMONDE";
    InitWindow(windowName, 1920, 1080);

    GameMemory gameMemory = AllocGameMemory(Megabytes(4));
    GameState *gameState = (GameState *) gameMemory.data;

    SetTargetFPS(60.0);

    gameState->renderArena = AllocArena(Megabytes(4));
    SavUseRenderArena(&gameState->renderArena);

    VertBatchSpec debugDrawSpec = VertBatchSpecBegin(DEFAULT_BATCH_MAX_VERT_COUNT, DEFAULT_BATCH_MAX_INDEX_COUNT, sizeof(u32));
    VertBatchSpecAddAttrib(&debugDrawSpec, DEFAULT_VERT_POSITIONS, false, 3, SAV_VA_TYPE_FLOAT, sizeof(v3));
    VertBatchSpecAddAttrib(&debugDrawSpec, DEFAULT_VERT_COLORS, false, 4, SAV_VA_TYPE_FLOAT, sizeof(v4));
    VertBatchSpecEnd(&debugDrawSpec);
    VertBatchPrepare(VERTBATCH_DEBUG_DRAW, debugDrawSpec);
    
    gameState->mainTileAtlas = SavLoadTextureAtlas("res/textures/hack64.png", 16, 16);
    SavSetTextureFilterMode(gameState->mainTileAtlas.texture, SAV_LINEAR);

    gameState->worldArena = AllocArena(Megabytes(4));

    f32 tilePxW = gameState->mainTileAtlas.cellW * LEVEL_ATLAS_SCALE;
    f32 tilePxH = gameState->mainTileAtlas.cellH * LEVEL_ATLAS_SCALE;
    gameState->level = MakeLevel(LEVEL_WIDTH, LEVEL_HEIGHT, &gameState->mainTileAtlas, tilePxW, tilePxH, &gameState->worldArena);
    gameState->entityStore = MakeEntityStore(ENTITY_STORE_COUNT, &gameState->worldArena, &gameState->mainTileAtlas, tilePxW, tilePxH);
    GenerateLevel(&gameState->level, &gameState->entityStore, LEVEL_CLASSIC_ROOMS);
    
    gameState->camera = MakeCamera(0.0f,
                                   GetWindowSize() / 2.0f,
                                   V2(tilePxW * gameState->entityStore.controlledEntity->p.x, tilePxH * gameState->entityStore.controlledEntity->p.y),
                                   0.2f,
                                   5.0f,
                                   5);
    
    CameraSetBounds(&gameState->camera, GetWindowSize().x, GetWindowSize().y, -0.5f * tilePxW, -0.5f * tilePxH, gameState->level.w * tilePxW, gameState->level.h * tilePxH);

    gameState->uiFont = SavLoadFont("res/fonts/VT323-Regular.ttf", 30);

    InitializeFloaterState(&gameState->uiFont, &gameState->camera, &gameState->worldArena);
    
    while (!WindowShouldClose())
    {
        BeginFrameTiming();

        PollEvents();

        if (KeyPressed(SDL_SCANCODE_B))
        {
            Breakpoint;
        }

        if (MouseWheel() != 0) CameraIncreaseLogZoomSteps(&gameState->camera, MouseWheel());
        if (MouseDown(SDL_BUTTON_MIDDLE)) CameraMoveTarget(&gameState->camera, CameraScreenToWorldRel(&gameState->camera, MouseRelPos()));

        SetWindowTitle(TextFormat("%s [%.2f fps]", windowName, GetFPSAvg()));

        switch (gameState->runState)
        {
            case RS_MAIN_MENU:
            {
                gameState->runState = RS_GAME_RUNNING;
            } break;

            case RS_GAME_RUNNING:
            {
                if (MousePressed(SDL_BUTTON_RIGHT))
                {
                    v2 target = CameraScreenToWorld(&gameState->camera, MousePos());
                    target.x = target.x / gameState->entityStore.tilePxW;
                    target.y = target.y / gameState->entityStore.tilePxH;
                    TraceLog("Target: %.3f, %.3f", target.x, target.y);
                    OrderEntityMovement(&gameState->entityStore, gameState->entityStore.controlledEntity, target);
                }

                if (MousePressed(SDL_BUTTON_LEFT))
                {
                    v2 p = CameraScreenToWorld(&gameState->camera, MousePos());
                    p.x = p.x / gameState->entityStore.tilePxW;
                    p.y = p.y / gameState->entityStore.tilePxH;
                    v2i tileP = V2I(RoundF32ToI32(p.x), RoundF32ToI32(p.y));
                    b32 blocked = IsTileBlocked(&gameState->level, tileP.x, tileP.y);
                    TraceLog("[%d, %d]: %d", tileP.x, tileP.y, blocked);
                }
                

                if (KeyDown(SDL_SCANCODE_A))
                {
                    gameState->entityStore.controlledEntity->yawDeg += 90.0f * (f32) GetDeltaFixed();
                }

                if (KeyDown(SDL_SCANCODE_D))
                {
                    gameState->entityStore.controlledEntity->yawDeg -= 90.0f * (f32) GetDeltaFixed();
                }

                UpdateEntities(&gameState->entityStore, (f32) GetDeltaFixed());

                BeginDraw();
                    ClearBackground(SAV_COLOR_LIGHTBLUE);

                    BeginCameraMode(&gameState->camera);
                        DrawLevel(&gameState->level);
                        DrawEntities(&gameState->entityStore);
                    EndCameraMode();

                    DrawFloaters((f32) GetDeltaFixed());
                    
                    DrawEntityUI(&gameState->entityStore, &gameState->uiFont, &gameState->worldArena);
                EndDraw();
            } break;
        }

        SavSwapBuffers();

        EndFrameTiming();
    }

    SavFreeFont(&gameState->uiFont);
    
    Quit();

    return 0;
}

#include "level.cpp"
#include "draw.cpp"
#include "tilemap.cpp"
#include "entity.cpp"
#include "navigation.cpp"
