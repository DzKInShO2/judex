#include <stdint.h>

#include <raylib.h>

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include <raylib-nuklear.h>

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
    RenderTexture2D tileset_framebuffer = LoadRenderTexture(screen_width * 0.2f, screen_height * 0.4f);
    Camera2D tileset_viewport = { 0 };
    tileset_viewport.zoom = 3.0f;

    TileMap tilemap = tilemap_create(60, 20, 8, 8, 6);
    Texture2D texture = LoadTexture("res/default_tileset.png");

    tileset_load(&tileset, &texture, 8, 8);

    Camera2D viewport = { 0 };
    viewport.zoom = 2.0f;
    viewport.offset.x = screen_width/3.0f;
    viewport.offset.y = screen_height/3.0f;

    struct nk_context *ctx = InitNuklear(14);

    u8 layer_current = 0;
    while (!WindowShouldClose()) {
        /* Update */
        // Update Window Size Variables
        if (IsWindowResized()) {
            screen_width = GetScreenWidth();
            screen_height = GetScreenHeight();

            viewport.offset.x = screen_width/3.0f;
            viewport.offset.y = screen_height/3.0f;

            UnloadRenderTexture(tileset_framebuffer);
            tileset_framebuffer = LoadRenderTexture(screen_width * 0.2f, screen_height * 0.4f);
        }

        // Update Nuklear
        UpdateNuklear(ctx);

        if (nk_begin(ctx, "Options",
                     nk_rect(1, (screen_height * 0.4f), (screen_width * 0.2f), (screen_height * 0.6f)),
                     NK_WINDOW_BORDER)) {
        }
        nk_end(ctx);

        // Handle Camera Movement
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 mouse_delta = GetMouseDelta();

            viewport.target.x -= mouse_delta.x * 0.5f * (1/viewport.zoom * 3.0f);
            viewport.target.y -= mouse_delta.y * 0.5f * (1/viewport.zoom * 3.0f);
        }
        viewport.zoom += GetMouseWheelMove() * viewport.zoom * 0.25f;

        // Handle Tile Placement
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 cursor = GetMousePosition();
            if (cursor.x > (screen_width * 0.205f)) {
                Vector2 pos = grid_get_position(
                    GetScreenToWorld2D(cursor, viewport),
                    tilemap.tilewidth, tilemap.tileheight
                );
                if (grid_is_position_valid(pos, tilemap.width, tilemap.height)) {
                    tilemap_set_tile(&tilemap, pos.x, pos.y, layer_current, tileset.active + 1);
                }
            } else {
                Vector2 pos = grid_get_position(
                    GetScreenToWorld2D(cursor, tileset_viewport),
                    tileset.tilewidth, tileset.tileheight
                );
                if (grid_is_position_valid(pos, tileset.width, tileset.height)) {
                    tileset_set_active(&tileset, pos.x, pos.y);
                }
            }
        }
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT)) {
            Vector2 cursor = GetMousePosition();
            if (cursor.x > (screen_width * 0.205f)) {
                Vector2 pos = grid_get_position(
                    GetScreenToWorld2D(cursor, viewport),
                    tilemap.tilewidth, tilemap.tileheight
                );
                if (grid_is_position_valid(pos, tilemap.width, tilemap.height)) {
                    tilemap_set_tile(&tilemap, pos.x, pos.y, layer_current, 0);
                }
            }
        }

        /* Draw */
        // TileSet Draw
        BeginTextureMode(tileset_framebuffer);
        ClearBackground(TILESET_COLOR);
        BeginMode2D(tileset_viewport);
        tileset_draw(&tileset);
        grid_draw(tileset.width, tileset.height,
                  tileset.tilewidth, tileset.tileheight);
        EndMode2D();
        EndTextureMode();

        // Main Draw
        BeginDrawing();
        ClearBackground(BACKGROUND_COLOR);

        BeginMode2D(viewport);
        // Draw Tilemap Content
        if (tileset.texture) {
            tilemap_draw_layer(&tilemap, &tileset, layer_current);
        }

        // Draw Tilemap Grid
        grid_draw(tilemap.width, tilemap.height,
                  tilemap.tilewidth, tilemap.tileheight);
        EndMode2D();

        DrawTextureRec(
            tileset_framebuffer.texture,
            (Rectangle){ 0, 0, tileset_framebuffer.texture.width, -tileset_framebuffer.texture.height},
            (Vector2){ 0, 0 },
            WHITE
        );
        DrawNuklear(ctx);
        EndDrawing();
    }

    tileset_unload(&tileset);
    UnloadTexture(texture);
    UnloadRenderTexture(tileset_framebuffer);

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
