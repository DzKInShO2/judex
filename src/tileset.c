#include "tileset.h"

#include <raylib.h>

#include "colorscheme.h"

void tileset_load(TileSet *tileset, Texture2D *texture, u16 tilewidth, u16 tileheight)
{
    tileset->tilewidth = tilewidth;
    tileset->tileheight = tilewidth;

    tileset->active = 0;

    if (texture != NULL) {
        tileset->texture = texture;
        tileset->width = texture->width/tileset->tilewidth;
        tileset->height = texture->height/tileset->tileheight;

        tileset->slices = calloc(tileset->width * tileset->height, sizeof(*tileset->slices));
        for (u16 j = 0; j < tileset->height; ++j) {
            for (u16 i = 0; i < tileset->width; ++i) {
                tileset->slices[(j * tileset->width) + i] = (Rectangle){
                    i * tilewidth, j * tileheight,
                    tilewidth, tileheight
                };
            }
        }
    }
}

void tileset_draw(TileSet *tileset)
{
    if (tileset->texture != NULL) {
        DrawTexture(*tileset->texture, 0, 0, WHITE);
        DrawRectangle(
            (u32)(tileset->active % tileset->width) * tileset->tilewidth,
            (u32)(tileset->active / tileset->width) * tileset->tileheight,
            tileset->tilewidth, tileset->tileheight,
            SELECTION_COLOR
        );
    }
}

void tileset_set_active(TileSet *tileset, u16 x, u16 y)
{
    tileset->active = (y * tileset->width) + x;
}

void tileset_unload(TileSet *tileset)
{
    if (tileset->texture != NULL)
        free(tileset->slices);
    tileset->texture = NULL;

    tileset->tilewidth  = 0;
    tileset->tileheight = 0;

    tileset->width  = 0;
    tileset->height = 0;
}
