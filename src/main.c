#include <ctype.h>
#include <stdint.h>
#include <limits.h>
#include <libgen.h>
#include <sys/stat.h>

#include <raylib.h>

#define RAYLIB_NUKLEAR_IMPLEMENTATION
#include <raylib-nuklear.h>

#include <sfd.h>

#include "common.h"
#include "tilemap.h"
#include "tileset.h"
#include "colorscheme.h"

#define SHORTCUT(a, b) (IsKeyDown(KEY_##a) && IsKeyPressed(KEY_##b))

typedef struct {
    i32 tilewidth, tileheight;
    char texture_path[1024];
} TileSetProperty;

void grid_draw(u16 width, u16 height, u16 tilewidth, u16 tileheight);
Vector2 grid_get_position(Vector2 world_position, u16 tilewidth, u16 tileheight);
bool grid_is_position_valid(Vector2 grid_position, u16 width, u16 height);

void judex_load_file(const char *path, TileMap *tilemap, TileSet *tileset, Texture2D *texture, TileSetProperty *tileset_property);
void judex_save_file(const char *path, const TileMap *tilemap, const TileSetProperty *tileset_property);

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
    SetTargetFPS(60);

    struct nk_context *ctx = InitNuklear(14);

    TileSet tileset = { 0 };
    RenderTexture2D tileset_framebuffer = LoadRenderTexture(panel_width, panel_offy);
    Camera2D tileset_viewport = { 0 };
    tileset_viewport.zoom = 3.0f;
    tileset_viewport.target.x -= 5.0f;
    tileset_viewport.target.y -= 5.0f;

    TileMap tilemap = tilemap_create(60, 20, 8, 8, 6);
    Texture2D texture;

    tileset_load(&tileset, NULL, 8, 8);

    Camera2D viewport = { 0 };
    viewport.zoom = 2.0f;
    viewport.offset.x = screen_width/3.0f;
    viewport.offset.y = screen_height/3.0f;

    TileSetProperty tileset_property = { 
        8, 8,
        ""
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

    sfd_Options file_save_opt = {
        .title = "Save Tilemap",
        .extension = "jdx",
        .filter_name = "JuDex Tilemap File",
        .filter = "*.jdx"
    };

    /* Editor Property */
    u8 layer_current = 0;
    bool layer_is_all_visible = false;
    bool grid_is_visible = true;
    char save_path[1024] = "";

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
                const char *path = (char *)sfd_open_dialog(&file_open_opt);
                strcpy(tileset_property.texture_path, path);

                struct stat path_stat; 
                stat(tileset_property.texture_path, &path_stat);
                if (strcmp(tileset_property.texture_path, "") != 0
                    && S_ISREG(path_stat.st_mode)) {
                    UnloadTexture(texture);
                    texture = LoadTexture(tileset_property.texture_path);

                    tileset_unload(&tileset);
                    tileset_load(&tileset, &texture, tileset_property.tilewidth, tileset_property.tileheight);
                }
            }

            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Apply Config"))  {
                if (strcmp(tileset_property.texture_path, "") != 0) {
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
            if (nk_button_label(ctx, "Load") || SHORTCUT(LEFT_CONTROL, O))  {
                const char *path = sfd_open_dialog(&file_save_opt);

                struct stat path_stat; 
                stat(path, &path_stat);

                if (path != NULL && S_ISREG(path_stat.st_mode)) {
                    judex_load_file(path, &tilemap, &tileset, &texture, &tileset_property);
                    strcpy(save_path, path);

                    tilemap_property.width = tilemap.width;
                    tilemap_property.height = tilemap.height;
                    tilemap_property.tilewidth = tilemap.tilewidth;
                    tilemap_property.tileheight = tilemap.tileheight;
                    tilemap_property.layer_count = tilemap.layer_count;
                }
            }
            if (nk_button_label(ctx, "Save") || SHORTCUT(LEFT_CONTROL, S))  {
                if (save_path[0] != '\0') {
                    judex_save_file(save_path, &tilemap, &tileset_property);
                } else {
                    const char *path = sfd_save_dialog(&file_save_opt);
                    strcpy(save_path, path);

                    struct stat path_stat; 
                    stat(save_path, &path_stat);

                    if (save_path[0] != '\0' && S_ISREG(path_stat.st_mode)) judex_save_file(save_path, &tilemap, &tileset_property);
                    else save_path[0] = '\0';
                }
            }
            
            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "Save As"))  {
                const char *path = sfd_save_dialog(&file_save_opt);
                strcpy(save_path, path);

                struct stat path_stat; 
                stat(save_path, &path_stat);

                if (save_path[0] != '\0' && S_ISREG(path_stat.st_mode)) judex_save_file(save_path, &tilemap, &tileset_property);
                else save_path[0] = '\0';
            }

            nk_layout_row_dynamic(ctx, 30, 1);
            if (nk_button_label(ctx, "New Map"))  {
                tilemap_destroy(&tilemap);
                tilemap = tilemap_create(tilemap_property.width, tilemap_property.height,
                                         tilemap_property.tilewidth, tilemap_property.tileheight,
                                         tilemap_property.layer_count);
                save_path[0] = '\0';
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
        if (SHORTCUT(LEFT_CONTROL, SPACE)) grid_is_visible = !grid_is_visible;
        // Toggle All Layer Visible
        if (SHORTCUT(LEFT_CONTROL, A)) layer_is_all_visible = !layer_is_all_visible;

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

void judex_load_file(const char *path, TileMap *tilemap, TileSet *tileset, Texture2D *texture, TileSetProperty *tileset_property)
{
    FILE *fp = fopen(path, "r");
    if (fp == NULL) {
        TraceLog(LOG_ERROR, "Failed to read file %s", path);
        return;
    }

    char *line = NULL;
    ssize_t len = 0;
    size_t size = 0;

    if ((len = getline(&line, &size, fp)) != -1) {
        u16 width, height;
        u16 tilewidth, tileheight;
        u8 layer_count;
        sscanf(line, "%hu, %hu, %hu, %hu, %hhu",
               &width, &height,
               &tilewidth, &tileheight,
               &layer_count);
        tilemap_change_property(tilemap, width, height, tilewidth, tileheight, layer_count);
    } else {
        TraceLog(LOG_ERROR, "Can't read file %s", path);
        fclose(fp);
        if (line != NULL)
            free((void *)line);
        return;
    }

    for (u8 i = 0; i < tilemap->layer_count; ++i) {
        if ((len = getline(&line, &size, fp)) != -1) {
            u32 l = (i * tilemap->width * tilemap->height);

            char *token = " ";
            for (u16 x = 0; x < tilemap->width && token != NULL; ++x) {
                for (u16 y = 0; y < tilemap->height && token != NULL; ++y) {
                    if (x == 0 && y == 0) token = strtok(line, ", ");
                    else token = strtok(NULL, ", ");
                    tilemap->layers[l + ((y * tilemap->width) + x)] = atoi(token);
                }
            }
        }
    }

    if ((len = getline(&line, &size, fp)) != -1) {
        u16 tilewidth = atoi(strtok(line, ", "));
        u16 tileheight = atoi(strtok(NULL, ", "));

        if ((len = getline(&line, &size, fp)) != -1) {
            UnloadTexture(*texture);
            *texture = LoadTexture(line);

            tileset_unload(tileset);
            tileset_load(tileset, texture, tilewidth, tileheight);

            tileset_property->tilewidth = tileset->tilewidth;
            tileset_property->tileheight = tileset->tileheight;
            strcpy(tileset_property->texture_path, line);
        } else {
            TraceLog(LOG_WARNING, "Can't read tileset path in file %s", path);
        }
    } else {
        TraceLog(LOG_WARNING, "Can't read tileset in file %s", path);
    }

    if (line != NULL)
        free((void *)line);

    fclose(fp);
    TraceLog(LOG_INFO, "Tilemap [%s] Successfully Loaded", path);
}

void judex_save_file(const char *path, const TileMap *tilemap, const TileSetProperty *tileset_property)
{
    FILE *fp = fopen(path, "w");
    if (fp == NULL) {
        TraceLog(LOG_ERROR, "Failed to write file: %s", path);
        return;
    }

    fprintf(fp, "%i, %i, %i, %i, %i\n",
            tilemap->width, tilemap->height,
            tilemap->tilewidth, tilemap->tileheight,
            tilemap->layer_count);

    for (u8 i = 0; i < tilemap->layer_count; ++i) {
         u32 l = i * tilemap->width * tilemap->height;
         for (u16 x = 0; x < tilemap->width; ++x) {
            for (u16 y = 0; y < tilemap->height; ++y) {
                if (x == 0 && y == 0) {
                    fprintf(fp, "%i", tilemap->layers[l + ((y * tilemap->width) + x)]);
                    continue;
                }
                fprintf(fp, ", %i", tilemap->layers[l + ((y * tilemap->width) + x)]);
            }
        }
        fprintf(fp, "\n");
    }

    fprintf(fp, "%i, %i\n", tileset_property->tilewidth, tileset_property->tileheight);
    fprintf(fp, "%s", tileset_property->texture_path);

    fclose(fp);
    TraceLog(LOG_INFO, "File Successfully Saved at %s", path);
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
