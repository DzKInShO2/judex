#include "tileset.h"

#include <raylib.h>

#include "colorscheme.h"

void tileset_load(TileSet *tileset, Texture2D *texture, u16 tilewidth, u16 tileheight)
{
    tileset->tilewidth = tilewidth;
    tileset->tileheight = tilewidth;
    tileset->width = texture->width/tileset->tilewidth;
    tileset->height = texture->height/tileset->tileheight;

    tileset->active = 0;
    tileset->texture = texture;

    tileset->slices = calloc(tileset->width * tileset->height, sizeof(*tileset->slices));
    for (u16 i = 0; i < tileset->width; ++i) {
        for (u16 j = 0; j < tileset->height; ++j) {
            tileset->slices[(j * tileset->width) + i] = (Rectangle){
                i * tilewidth, j * tileheight,
                tilewidth, tileheight
            };
        }
    }
}

void tileset_draw(TileSet *tileset)
{
    DrawTexture(*tileset->texture, 0, 0, WHITE);
    DrawRectangle(
        (u32)(tileset->active % tileset->width),
        (u32)(tileset->active / tileset->width),
        tileset->tilewidth, tileset->tileheight,
        SELECTION_COLOR
    );
}

void tileset_unload(TileSet *tileset)
{
    tileset->texture = NULL;
    free(tileset->slices);
}