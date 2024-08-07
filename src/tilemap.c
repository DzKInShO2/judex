#include "tilemap.h"

#include <raylib.h>

void tilemap_copy_layer(u8 layer, u16 *dl, u16 dw, u16 dh, u16 *sl, u16 sw, u16 sh)
{
    u32 dlen = layer * (dw * dh);
    u32 slen = layer * (sw * sh);

    for (u16 y = 0; y < (dh > sh ? sh : dh); ++y) {
        for (u16 x = 0; x < (dw > sw ? sw : dw); ++x) {
            dl[((y * dw) + x) + dlen] = sl[((y * sw) + x) + slen];
        }
    }
}

TileMap tilemap_create(u16 width, u16 height, u16 tilewidth, u16 tileheight, u8 layer_count)
{
    TileMap tilemap = { 0 };
    tilemap.width = width;
    tilemap.height = height;
    tilemap.tilewidth = tilewidth;
    tilemap.tileheight = tileheight;
    tilemap.layer_count = layer_count;
    tilemap.layers = calloc(width * height * layer_count, sizeof(*tilemap.layers));

    return tilemap;
}

void tilemap_change_property(TileMap *tilemap, u16 width, u16 height, u16 tilewidth, u16 tileheight, u8 layer_count)
{
    bool is_regenerated = false;

    if (tilemap->tilewidth != tilewidth) tilemap->tilewidth = tilewidth;
    if (tilemap->tileheight != tileheight) tilemap->tileheight = tileheight;

    if (tilemap->width != width || tilemap->height != height || tilemap->layer_count != layer_count)
        is_regenerated = true;

    if (is_regenerated) {
        u16 *new_layers = calloc(width * height * layer_count, sizeof(*new_layers));

        for (u8 i = 0; i < (layer_count > tilemap->layer_count ? tilemap->layer_count : layer_count); ++i) {
            tilemap_copy_layer(i, new_layers, width, height, tilemap->layers, tilemap->width, tilemap->height);
        }

        tilemap->width = width;
        tilemap->height = height;
        tilemap->layer_count = layer_count;

        free((void *)tilemap->layers);
        tilemap->layers = new_layers;
    }
}

void tilemap_set_tile(TileMap *tilemap, u16 x, u16 y, u8 layer, u16 id)
{
    tilemap->layers[(layer * tilemap->width * tilemap->height) + ((y * tilemap->width) + x)] = id;
}

void tilemap_draw_layer(TileMap *tilemap, TileSet *tileset, u8 layer)
{
    u32 k = (layer * tilemap->width * tilemap->height);
    for (u16 j = 0; j < tilemap->height; ++j) {
        for (u16 i = 0; i < tilemap->width; ++i) {
            u16 l = tilemap->layers[k + ((j * tilemap->width) + i)];

            if (l != 0) {
                DrawTextureRec(
                    *tileset->texture,
                    tileset->slices[l - 1],
                    (Vector2) { i * tilemap->tilewidth, j * tilemap->tileheight },
                    WHITE
                );
            }
        }
    }
}

void tilemap_draw_all_layers(TileMap *tilemap, TileSet *tileset)
{
    for (u8 i = 0; i < tilemap->layer_count; ++i) {
        tilemap_draw_layer(tilemap, tileset, i);
    }
}

void tilemap_destroy(TileMap *tilemap)
{
    free(tilemap->layers);
}
