#include <cstdio>

#include "sav.h"

int main(int argc, char **argv)
{
    const char *windowName = "SURMONDE";
    InitWindow(windowName, 1920, 1080);

    SetTargetFPS(60.0);

    while (!WindowShouldClose())
    {
        BeginFrameTiming();

        PollEvents();

        // RENDER
        SetWindowTitle(TextFormat("%s [%.2f fps]", windowName, GetFPSAvg()));

        EndFrameTiming();
    }

    Quit();

    return 0;
}