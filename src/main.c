#include <stdint.h>
#include <limits.h>
#include <sys/stat.h>

#include <raylib.h>

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include <raylib-nuklear.h>

#include <sfd.h>

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
    u32 panel_width = screen_width * 0.2f;
    u32 panel_height = screen_height * 0.6f;
    u32 panel_offy = screen_height * 0.4f;
    u32 panel_bound_x = screen_width * 0.205f;

    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(screen_width, screen_height, "JuDex");
    SetExitKey(KEY_NULL);

    struct nk_context *ctx = InitNuklear(14);

    TileSet tileset = { 0 };
    RenderTexture2D tileset_framebuffer = LoadRenderTexture(panel_width, panel_offy);
    Camera2D tileset_viewport = { 0 };
    tileset_viewport.zoom = 3.0f;
    tileset_viewport.target.x -= 5.0f;
    tileset_viewport.target.y -= 5.0f;

    TileMap tilemap = tilemap_create(60, 20, 8, 8, 6);
    Texture2D texture = LoadTexture("res/default_tileset.png");

    tileset_load(&tileset, &texture, 8, 8);

    Camera2D viewport = { 0 };
    viewport.zoom = 2.0f;
    viewport.offset.x = screen_width/3.0f;
    viewport.offset.y = screen_height/3.0f;

    struct {
        i32 tilewidth, tileheight;
        char *texture_path;
    } tileset_property = { 
        8, 8,
        NULL
    };

    struct {
        i32 width, height;
        i32 tilewidth, tileheight;
        i32 layer_count;
    } tilemap_property = {
        60, 20,
        8, 8,
        6
    };

    /* File Dialog Options */
    sfd_Options file_open_opt = {
        .title = "Open Image File",
        .filter_name = "Image File",
        .filter = "*.png|*.jpg"
    };

    /* Editor Property */
    u8 layer_current = 0;
    bool layer_is_all_visible = false;
    bool grid_is_visible = true;

    /* Main Loop */
    while (!WindowShouldClose()) {
        /* Update */
        // Update Window Size Variables
        if (IsWindowResized()) {
            screen_width = GetScreenWidth();
            screen_height = GetScreenHeight();

            panel_width = screen_width * 0.2f;
            panel_height = screen_height * 0.6f;

            panel_offy = screen_height * 0.4f;
            panel_bound_x = screen_width * 0.205f;

            viewport.offset.x = screen_width/3.0f;
            viewport.offset.y = screen_height/3.0f;

            UnloadRenderTexture(tileset_framebuffer);
            tileset_framebuffer = LoadRenderTexture(panel_width, panel_offy);
        }

        // Update Nuklear
        UpdateNuklear(ctx);

        if (nk_begin(ctx, "Options",
                     nk_rect(1, (panel_offy), panel_width, panel_height),
                     NK_WINDOW_BORDER)) {
            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Tileset Config", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 30, 2);
            nk_property_int(ctx, "Tile Width ", 0, &tileset_property.tilewidth, USHRT_MAX, 1, 1);
            nk_property_int(ctx, "Tile Height ", 0, &tileset_property.tileheight, USHRT_MAX, 1, 1);

            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Load Texture"))  {
                tileset_property.texture_path = (char *)sfd_open_dialog(&file_open_opt);

                struct stat path_stat; 
                stat(tileset_property.texture_path, &path_stat);
                if (tileset_property.texture_path != NULL
                    && S_ISREG(path_stat.st_mode)) {
                    UnloadTexture(texture);
                    texture = LoadTexture(tileset_property.texture_path);

                    tileset_unload(&tileset);
                    tileset_load(&tileset, &texture, tileset_property.tilewidth, tileset_property.tileheight);
                }
            }

            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Apply Config"))  {
                if (tileset_property.texture_path != NULL) {
                    tileset_unload(&tileset);
                    tileset_load(&tileset, &texture, tileset_property.tilewidth, tileset_property.tileheight);
                }
            }

            nk_layout_row_dynamic(ctx, 60, 1);
            nk_spacer(ctx);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Tilemap Config", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 30, 2);
            nk_property_int(ctx, "Width", 0, &tilemap_property.width ,USHRT_MAX, 1, 1);
            nk_property_int(ctx, "Height", 0, &tilemap_property.height ,USHRT_MAX, 1, 1);

            nk_layout_row_dynamic(ctx, 30, 2);
            nk_property_int(ctx, "Tile Width", 0,  &tilemap_property.tilewidth ,USHRT_MAX, 1, 1);
            nk_property_int(ctx, "Tile Height", 0, &tilemap_property.tileheight ,USHRT_MAX, 1, 1);

            nk_layout_row_dynamic(ctx, 30, 1);
            nk_property_int(ctx, "Layer Count", 0, &tilemap_property.layer_count ,UCHAR_MAX, 1, 1);

            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Apply Config"))  {
                tilemap_change_property(&tilemap, tilemap_property.width, tilemap_property.height,
                                        tilemap_property.tilewidth, tilemap_property.tileheight,
                                        tilemap_property.layer_count);
            }

            nk_layout_row_dynamic(ctx, 60, 1);
            nk_spacer(ctx);

            nk_layout_row_dynamic(ctx, 20, 1);
            nk_label(ctx, "Load/Save Tilemap", NK_TEXT_CENTERED);

            nk_layout_row_dynamic(ctx, 30, 2);
            if (nk_button_label(ctx, "Load"))  {
            }
            if (nk_button_label(ctx, "Save"))  {
            }
            
            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Save As"))  {
            }
        }
        nk_end(ctx);

        // Get Cursor Position
        Vector2 cursor = GetMousePosition();

        // Handle Camera Movement
        if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
            Vector2 mouse_delta = GetMouseDelta();

            if (cursor.x > (panel_width)) {
                viewport.target.x -= mouse_delta.x * 0.5f * (1/viewport.zoom * 3.0f);
                viewport.target.y -= mouse_delta.y * 0.5f * (1/viewport.zoom * 3.0f);
            } else {
                if (cursor.y < (panel_offy)) {
                    tileset_viewport.target.x -= mouse_delta.x * 0.25f * (1/tileset_viewport.zoom * 3.0f);
                    tileset_viewport.target.y -= mouse_delta.y * 0.25f * (1/tileset_viewport.zoom * 3.0f);
                }
            }
        }
        if (IsKeyDown(KEY_LEFT_SHIFT)) {
            i16 layer_next = layer_current + (i8)GetMouseWheelMove();
            layer_current = layer_next >= 0 ? (layer_next < tilemap.layer_count ? layer_next : tilemap.layer_count - 1) : 0;
        } else {
            if (cursor.y < (panel_offy) && cursor.x < (panel_width))
                tileset_viewport.zoom += GetMouseWheelMove() * tileset_viewport.zoom * 0.15f;
            else viewport.zoom += GetMouseWheelMove() * viewport.zoom * 0.25f;
        }

        // Toggle Grid
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_SPACE)) grid_is_visible = !grid_is_visible;
        // Toggle All Layer Visible
        if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyPressed(KEY_A)) layer_is_all_visible = !layer_is_all_visible;

        // Handle Tile Placement
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            if (cursor.x > panel_bound_x && tileset.texture) {
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
        if (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) && tileset.texture) {
            if (cursor.x > panel_bound_x) {
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
            if (layer_is_all_visible) {
                for (u8 i = 0; i < tilemap.layer_count; ++i) {
                    tilemap_draw_layer(&tilemap, &tileset, i);
                }
            } else tilemap_draw_layer(&tilemap, &tileset, layer_current);
        }

        // Draw Tilemap Grid
        if (grid_is_visible)
            grid_draw(tilemap.width, tilemap.height,
                      tilemap.tilewidth, tilemap.tileheight);
        EndMode2D();

        // Draw Tileset View
        DrawTextureRec(
            tileset_framebuffer.texture,
            (Rectangle){ 0, 0, tileset_framebuffer.texture.width, -tileset_framebuffer.texture.height},
            (Vector2){ 0, 0 },
            WHITE
        );
        
        // Draw Layer Indicator
        DrawText(TextFormat("%i", layer_current + 1), panel_bound_x, 0, 32, (layer_is_all_visible ? ORANGE : WHITE));

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
