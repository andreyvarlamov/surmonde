#include "game_save.h"

#include "sav.h"
#include "surmonde.h"

api_func void SaveGame(GameState *gameState)
{
    SavFile file = SavOpenFile("temp/saves/test_save.txt", SavFileOpenType_Write);

    for (int levelI = 0; levelI < gameState->levelStore.levelCount; levelI++)
    {
        Level *level = gameState->levelStore.levels + levelI;
        Tilemap *tilemap = &level->levelTilemap;

        SavFilePrintFormat(file, "begin level %d, %d\n", level->worldPos.x, level->worldPos.y);
        for (int tileI = 0; tileI < level->w * level->h; tileI++)
        {
            SavFilePrintFormat(file,
                               "    tile %d, %0.3f, %0.3f, %0.3f, %0.3f, %u\n",
                               tilemap->atlasValues[tileI],
                               tilemap->colors[tileI].r,
                               tilemap->colors[tileI].g,
                               tilemap->colors[tileI].b,
                               tilemap->colors[tileI].a,
                               level->tileFlags[tileI]);
        }
        SavFilePrintFormat(file, "end\n");
    }

    SavCloseFile(&file);
}

api_func void LoadGame()
{
}

