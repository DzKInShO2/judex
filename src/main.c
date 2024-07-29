#include <stdint.h>

#include <raylib.h>

#include "common.h"
#include "tilemap.h"
#include "tileset.h"
#include "colorscheme.h"

void grid_draw(u16 width, u16 height, u16 tilewidth, u16 tileheight);
Vector2 grid_get_position(Vector2 world_position, u16 tilewidth, u16 tileheight);
bool grid_is_position_valid(Vector2 grid_position, u16 width, u16 height);

int main(void)
{
    u32 screen_width = 1920;
    u32 screen_height = 1080;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "JuDex");

    TileSet tileset = { 0 };
    TileMap tilemap = tilemap_create(60, 20, 8, 8, 6);
    Texture2D texture = LoadTexture("res/default_tileset.png");

    tileset_load(&tileset, &texture, 8, 8);

    u8 layer_current = 0;
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

        // Draw Tilemap Content
        if (tileset.texture) {
            tilemap_draw_layer(&tilemap, &tileset, layer_current);
        }

        // Draw Tilemap Grid
        grid_draw(tilemap.width, tilemap.height,
                  tilemap.tilewidth, tilemap.tileheight);

        EndDrawing();
    }

    tileset_unload(&tileset);
    UnloadTexture(texture);

    CloseWindow();
    return 0;
}

void grid_draw(u16 width, u16 height, u16 tilewidth, u16 tileheight)
{
    for (u16 i = 0; i < width + 1; ++i) {
        DrawLine(i * tilewidth, 0, i * tilewidth, height * tileheight, GRID_COLOR);
    }

    for (u16 j = 0; j < height + 1; ++j) {
        DrawLine(0, j * tileheight, width * tilewidth, j * tileheight, GRID_COLOR);
    }
}

Vector2 grid_get_position(Vector2 world_position, u16 tilewidth, u16 tileheight)
{
    return (Vector2){
        (u32)(world_position.x / tilewidth),
        (u32)(world_position.y / tileheight)
    };
}

bool grid_is_position_valid(Vector2 grid_position, u16 width, u16 height)
{
    return (grid_position.x >= 0 && grid_position.y >= 0)
            && (grid_position.x < width && grid_position.y < height);
}
