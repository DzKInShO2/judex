#ifndef _TILESET_H_
#define _TILESET_H_

#include "common.h"

typedef struct Rectangle Rectangle;

typedef struct {
    u16 width, height;
    u16 tilewidth, tileheight;

    u32 active;
    Rectangle *slices;
} TileSet;

void tileset_load(TileSet *tileset, const char *file_path, u16 tilewidth, u16 tileheight);
void tileset_unload(TileSet *tileset);

#endif //_TILESET_H_
