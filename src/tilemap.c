#include <tilemap.h>
#include <gfx.h>

void tilemap_render(int sx, int sy, Texture *tex, unsigned char *data,
    int map_w, int map_h) {
    int x, y;
    
    int tx = sx/TILE_W;
    int ty = sy/TILE_H;
    
    int px, py;
    
    int dx = -(sx%TILE_W);
    int dy = -(sy%TILE_H);
    
    unsigned char tile;
    int ax, ay;
    
    for(y=0;y<HEIGHT/TILE_H+2;y++){
        for(x=0;x<WIDTH/TILE_W+1;x++){
            px = tx+x;
            py = ty+y;
            if(px >= 0 && px < map_w && py >= 0 && py < map_h) {
                tile = data[py*map_w+px];
                ax = tile%ATLAS_W;
                ay = tile/ATLAS_W;
                if(ay > ATLAS_H) continue;
                gfx_draw_sprite_opt(tex, dx+x*TILE_W, dy+y*TILE_H, ax*TILE_W,
                    ay*TILE_H, TILE_W, TILE_H, TILE_W, TILE_H);
            }
        }
    }
}

