#include "surmonde.h"

#include <cstdio>
#include <sdl2/SDL_scancode.h>
#include <sdl2/SDL_mouse.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

#include "sav.h"

#include "level.h"
#include "defines.h"
#include "helpers.h"
#include "draw.h"
#include "debug_draw.h"
#include "inventory.h"
#include "sprites.h"

#include "ui_debug_actor.h"
#include "ui_debug_inventory_store.h"
#include "ui_inventory.h"

int main(int argc, char **argv)
{
    const char *windowName = "SURMONDE";
    InitWindow(windowName, 1920, 1080);

    GameMemory gameMemory = AllocGameMemory(Megabytes(4));
    GameState *gameState = (GameState *) gameMemory.data;

    SetTargetFPS(60.0);

    gameState->worldAtlas = SavLoadTextureAtlas("res/textures/monde_atlas.png", 16, 16);
    SavSetTextureFilterMode(gameState->worldAtlas.texture, SAV_NEAREST);

    gameState->worldArena = AllocArena(Megabytes(8));
    
    InitializeSprites(&gameState->spriteAtlasStore, &gameState->worldArena);
    InitializeInventoryItemSpecStore(&gameState->inventoryItemSpecStore, &gameState->worldArena);
    
    f32 tilePxW = gameState->worldAtlas.cellW * LEVEL_ATLAS_SCALE;
    f32 tilePxH = gameState->worldAtlas.cellH * LEVEL_ATLAS_SCALE;
    gameState->level = MakeLevel(LEVEL_WIDTH, LEVEL_HEIGHT, &gameState->worldAtlas, tilePxW, tilePxH, &gameState->worldArena);

    gameState->entityStore = MakeEntityStore(ENTITY_STORE_COUNT, &gameState->worldArena, &gameState->level);
    gameState->inventoryStore = MakeInventoryStore(INVENTORY_BLOCK_COUNT, &gameState->worldArena);
    GenerateLevel(&gameState->level, &gameState->entityStore, LEVEL_CLASSIC_ROOMS);
    
    gameState->camera = MakeCamera(0.0f,
                                   GetWindowSize() / 2.0f,
                                   V2(tilePxW * gameState->entityStore.controlledEntity->p.x, tilePxH * gameState->entityStore.controlledEntity->p.y),
                                   0.2f,
                                   6.0f,
                                   7);
    
    CameraSetBounds(&gameState->camera, GetWindowSize().x, GetWindowSize().y, -0.5f * tilePxW, -0.5f * tilePxH, gameState->level.w * tilePxW, gameState->level.h * tilePxH);
    CameraSetBounds(&gameState->camera, GetWindowSize().x, GetWindowSize().y, 0.0f, 0.0f, gameState->level.w * tilePxW, gameState->level.h * tilePxH);

    gameState->uiFont = SavLoadFont("res/fonts/VT323-Regular.ttf", 30);

    InitializeFloaterState(&gameState->uiFont, &gameState->camera, &gameState->worldArena);

    gameState->debugRenderArena = AllocArena(Megabytes(4));
    DDrawInit(&gameState->debugRenderArena);

    Tile floorTile = MakeTile(4, V4(1,1,1,1), 0);
    Tile wallTile = MakeTile(3, V4(1,1,1,1), TILE_BLOCKED | TILE_OPAQUE);

    int debugEdge = 0;
    b32 debugEdgeTiles = false;

    gameState->inventoryOpen = true;
    gameState->viewedEntities[0] = gameState->entityStore.controlledEntity;
        
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

        SetWindowTitle(TextFormat("%s %s[%.2f fps]", windowName, gameState->isPaused ? "[PAUSED] " : "", GetFPSAvg()));

        ImGui::ShowDemoWindow();

        switch (gameState->runState)
        {
            case RS_MAIN_MENU:
            {
                gameState->runState = RS_GAME_RUNNING;
            } break;

            case RS_GAME_RUNNING:
            {
                if (!gameState->isPaused)
                {
                    if (MousePressed(SDL_BUTTON_LEFT) && KeyDown(SDL_SCANCODE_LCTRL))
                    {
                        v2 target = CameraScreenToWorld(&gameState->camera, MousePos());
                        target.x = target.x / gameState->entityStore.tilePxW;
                        target.y = target.y / gameState->entityStore.tilePxH;
                        Entity *clickedEntity = GetEntityOfTypeAt(&gameState->entityStore, ENTITY_TYPE_ACTOR, target).entities[0];
                        if (clickedEntity != NULL && !IsControlledEntity(&gameState->entityStore, clickedEntity))
                        {
                            gameState->entityStore.controlledEntity = clickedEntity;
                            ResetActorAI(clickedEntity);
                        }
                    }
                    else if (gameState->entityStore.controlledEntity != NULL && MouseDown(SDL_BUTTON_LEFT))
                    {
                        v2 target = CameraScreenToWorld(&gameState->camera, MousePos());
                        target.x = target.x / gameState->entityStore.tilePxW;
                        target.y = target.y / gameState->entityStore.tilePxH;
                    
                        Entity *clickedEntity = GetEntityOfTypeAt(&gameState->entityStore, ENTITY_TYPE_ACTOR, target).entities[0];

                        if (clickedEntity != NULL && !IsControlledEntity(&gameState->entityStore, clickedEntity))
                        {
                            gameState->entityStore.controlledEntity->aiState.type = ACTOR_AI_FOLLOW_ENTITY;
                            gameState->entityStore.controlledEntity->aiState.entityToFollow = clickedEntity;
                        }
                        else
                        {
                            gameState->entityStore.controlledEntity->aiState.type = ACTOR_AI_MOVE_TO_TARGET;
                            gameState->entityStore.controlledEntity->aiState.movementTarget = target;
                        }
                    }
                }

                if (MousePressed(SDL_BUTTON_RIGHT))
                {
                    v2 p = CameraScreenToWorld(&gameState->camera, MousePos());
                    p.x = p.x / gameState->entityStore.tilePxW;
                    p.y = p.y / gameState->entityStore.tilePxH;
                    Entity *clickedEntity = GetEntityAt(&gameState->entityStore, p).entities[0];
                    b32 disabled = false;
                    Entity **addToThisSlot = NULL;
                    for (int i = 0; i < VIEWED_ENTITY_MAX; i++)
                    {
                        Entity **viewedEntitySlot = gameState->viewedEntities + i;
                        if (*viewedEntitySlot == clickedEntity)
                        {
                            *viewedEntitySlot = NULL;
                            disabled = true;
                        }
                        else if (*viewedEntitySlot == NULL)
                        {
                            addToThisSlot = viewedEntitySlot;
                        }
                    }
                    if (!disabled && addToThisSlot != NULL)
                    {
                        *addToThisSlot = clickedEntity;
                    }
                }

                if (KeyPressed(SDL_SCANCODE_SPACE))
                {
                    gameState->isPaused = !gameState->isPaused;
                }

                if (KeyPressed(SDL_SCANCODE_I))
                {
                    gameState->inventoryOpen = !gameState->inventoryOpen;
                }

                if (!gameState->isPaused)
                {
                    UpdateEntities(&gameState->entityStore, (f32) GetDeltaFixed());
                }

                BeginDraw();
                    ClearBackground(SAV_COLOR_LIGHTBLUE);

                    BeginCameraMode(&gameState->camera);
                        DrawLevel(&gameState->level);
                        DrawLevelOcclusion(&gameState->level, gameState->entityStore.controlledEntityVisibleTiles);
                        DrawEntities(&gameState->entityStore);

                        DDraw();
                    EndCameraMode();

                    DrawFloaters((f32) GetDeltaFixed());
                EndDraw();

                for (int i = 0; i < VIEWED_ENTITY_MAX; i++)
                {
                    Entity *viewedEntity = gameState->viewedEntities[i];
                    if (viewedEntity != NULL)
                    {
                        DrawDebugActorUI(&gameState->entityStore, &gameState->inventoryStore, viewedEntity);
                    }
                }

                DrawDebugInventoryStoreUI(&gameState->inventoryStore);

                if (gameState->inventoryOpen)
                {
                    DrawInventoryUI(&gameState->inventoryStore, gameState->entityStore.controlledEntity);
                }
            } break;
        }

        MemoryArenaResetScratch();
        
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
#include "debug_draw.cpp"
#include "vision.cpp"
#include "inventory.cpp"
#include "sprites.cpp"
#include "inventory_item.cpp"
#include "inventory_item_spec.cpp"


#include "ui_debug_actor.cpp"
#include "ui_debug_inventory_store.cpp"
#include "ui_inventory.cpp"
