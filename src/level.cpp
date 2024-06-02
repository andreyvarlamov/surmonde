#include "level.h"
#include "sav.h"
#include "settings.h"

api_func Level MakeLevel(MemoryArena *mem, int w, int h, SavTextureAtlas *atlas, f32 atlasScale)
{
    Level level;
    level.w = w;
    level.h = h;
    level.tiles = MemoryArenaPushArrayAndZero(mem, w * h, i32);
    level.tilePxW = atlas->cellW * atlasScale;
    level.tilePxH = atlas->cellH * atlasScale;
    level.atlas = atlas;
    return level;
}

api_func void DrawLevel(Level *level)
{
    for (int i = 0; i < level->w * level->h; i++)
    {
        i32 *tile = level->tiles + i;
        if (GetCurrentFrame() % 5 == 0)
        {
            if (GetRandomValue(0, 10) == 0)
            {
                *tile = GetRandomValue(0, 256);
            }
        }

        i32 tileVal = level->tiles[i];
        i32 tileX = i % level->w;
        i32 tileY = i / level->h;
        Rect destRect = MakeRect(tileX * level->tilePxW, tileY * level->tilePxH, level->tilePxW, level->tilePxH);
        DrawAtlasCell(*level->atlas, tileVal % 16, tileVal / 16, SAV_COLOR_CROWBLACK, SAV_COLOR_ASHGRAY, destRect);
    }
}