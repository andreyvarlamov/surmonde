#include "level.h"
#include "sav.h"
#include "settings.h"

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale)
{
    Level level;
    level.w = w;
    level.h = h;
    level.tiles = MemoryArenaPushArrayAndZero(mem, w * h, Tile);
    level.tilePxW = atlas->cellW * atlasScale;
    level.tilePxH = atlas->cellH * atlasScale;
    level.atlas = atlas;
    return level;
}

api_func void DrawLevel(Level *level)
{
    for (int i = 0; i < level->w * level->h; i++)
    {
        Tile *tile = level->tiles + i;
        if (GetCurrentFrame() % 5 == 0)
        {
            if (GetRandomValue(0, 10) == 0)
            {
                tile->atlasValue = GetRandomValue(0, 256);
            }
        }

        i32 tileVal = tile->atlasValue;
        i32 tileX = i % level->w;
        i32 tileY = i / level->h;
        Rect destRect = MakeRect(tileX * level->tilePxW, tileY * level->tilePxH, level->tilePxW, level->tilePxH);

        // SAV_COLOR_SABLE, SAV_COLOR_ASHGRAY
        // SAV_COLOR_SABLE, SAV_COLOR_MIDNIGHT
        // SAV_COLOR_SABLE, SAV_COLOR_LIGHTGOLDENRODYELLOW
        DrawRect(destRect, SAV_COLOR_SABLE);
        DrawAtlasCell(*level->atlas, tileVal % 16, tileVal / 16, destRect, SAV_COLOR_ASHGRAY);

        // DrawAtlasTileMap(*level->atlas, level->tiles, level->tilesBg, level->tilesFg, level->tilePxW, level->tilePxH);
    }
}