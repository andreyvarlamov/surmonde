#include "surmonde.h"

#include <cstdio>

#include "sav.h"

int main(int argc, char **argv)
{
    const char *windowName = "SURMONDE";
    InitWindow(windowName, 1920, 1080);

    GameMemory gameMemory = AllocGameMemory(Megabytes(4));
    GameState *gameState = (GameState *) gameMemory.data;

    SetTargetFPS(60.0);

    SavTexture texture = SavLoadTexture("res/textures/hack64.png");

    while (!WindowShouldClose())
    {
        BeginFrameTiming();

        PollEvents();

        // RENDER
        SetWindowTitle(TextFormat("%s [%.2f fps]", windowName, GetFPSAvg()));

        BeginDraw();
            ClearBackground(SAV_COLOR_LIGHTBLUE);

            DrawRect(MakeRect(10.0f, 10.0f, 100.0f, 100.0f), SAV_COLOR_GREEN);

            // DrawWorld();
            int atlasHorizontalCount = 16;
            int atlasVerticalCount = 16;
            int atlasCellW = texture.w / atlasHorizontalCount;
            int atlasCellH = texture.h / atlasVerticalCount;

            int atlasX = 1;
            int atlasY = 1;
            int atlasPxX = atlasX * atlasCellW;
            int atlasPxY = atlasY * atlasCellH;
            Rect atlasRect = MakeRect((f32) atlasPxX, (f32) atlasPxY, (f32) atlasCellW, (f32) atlasCellH);
            DrawTexture(texture, MakeRect(10.0f, 10.0f, 100.0f, 100.0f), atlasRect, {}, 0.0f, SAV_COLOR_BLACK);
        EndDraw();

        SavSwapBuffers();

        EndFrameTiming();
    }

    Quit();

    return 0;
}