#include <stdint.h>

#include <raylib.h>

#include "colorscheme.h"

int main(void)
{
    uint32_t screen_width = 1920;
    uint32_t screen_height = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "JuDex");

    while (!WindowShouldClose()) {
        /* Update */
        // Update Window Size Variables
        if (IsWindowResized()) {
            screen_width = GetScreenWidth();
            screen_height = GetScreenHeight();
        }

        /* Draw */
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}
