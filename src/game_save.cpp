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
        size_t count = (size_t)level->w * (size_t)level->h;

        SavFilePrintFormat(file, "level %d, %d\n", level->worldPos.x, level->worldPos.y);

        SavFilePrintFormat(file, "atlas values:\n");
        SavFileWriteBytes(file, tilemap->atlasValues, count * sizeof(*tilemap->atlasValues));

        SavFilePrintFormat(file, "\ncolors:\n");
        SavFileWriteBytes(file, tilemap->colors, count * sizeof(*tilemap->colors));

        SavFilePrintFormat(file, "\nflags:\n");
        SavFileWriteBytes(file, level->tileFlags, count * sizeof(*level->tileFlags));
        
        SavFilePrintFormat(file, "\nend\n");
    }

    SavCloseFile(&file);
}

api_func void LoadGame()
{
}

