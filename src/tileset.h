#ifndef _TILESET_H_
#define _TILESET_H_

#include "common.h"

typedef struct Texture Texture2D;
typedef struct Rectangle Rectangle;

typedef struct {
    u16 width, height;
    u16 tilewidth, tileheight;

    u32 active;
    Rectangle *slices;
    Texture2D *texture;
} TileSet;

void tileset_load(TileSet *tileset, Texture2D *texture, u16 tilewidth, u16 tileheight);
void tileset_draw(TileSet *tileset);
void tileset_set_active(TileSet *tileset, u16 x, u16 y);
void tileset_unload(TileSet *tileset);

#endif //_TILESET_H_
