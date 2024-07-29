#ifndef _TILEMAP_H_
#define _TILEMAP_H_

#include "common.h"

#include "tileset.h"

typedef struct {
    u16 width, height;
    u16 tilewidth, tileheight;

    u8 layer_count;
    u16 *layers;
} TileMap;

TileMap tilemap_create(u16 width, u16 height, u16 tilewidth, u16 tileheight, u8 layer_count);
void tilemap_change_property(TileMap *tilemap, u16 width, u16 height, u16 tilewidth, u16 tileheight, u8 layer_count);
void tilemap_set_tile(TileMap *tilemap, u16 x, u16 y, u8 layer, u16 id);
void tilemap_draw_layer(TileMap *tilemap, TileSet *tileset, u8 layer);
void tilemap_draw_all_layers(TileMap *tilemap, TileSet *tileset);
void tilemap_destroy(TileMap *tilemap);

#endif //_TILEMAP_H_
