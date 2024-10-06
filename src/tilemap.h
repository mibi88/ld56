#ifndef TILEMAP_H
#define TILEMAP_H

#define ATLAS_W 4
#define ATLAS_H 4

#define TILE_W 16
#define TILE_H 16

#include <gfx.h>

void tilemap_render(int sx, int sy, Texture *tex, unsigned char *data,
    int map_w, int map_h);

#endif

