#include <overview.h>
#define STB_PERLIN_IMPLEMENTATION
#include <stb/stb_perlin.h>

#include <stdlib.h>

#include <string.h>

#include <limits.h>

#define TILE_AMOUNT 3

#define ANT_NUM 200

#define ATTACK_DIST 16
#define HILL_RADIUS 64

#define PI 3.14159

const Entity default_ant = {0, 0, 0, 0, 0, 0, 0, A_NONE, 0, T_ANT, 0, 100, 5,
    M_NONE, NULL, 0, NULL};

unsigned int xorshift(unsigned int *seed) {
    *seed ^= *seed << 13;
    *seed ^= *seed >> 17;
    *seed ^= *seed << 5;
    return *seed;
}

int overview_generate(unsigned char *tilemap, Anthill *anthills, int hill_num,
    int width, int height, unsigned int seed, Texture *player, Texture *enemy1,
    Texture *enemy2) {
    int x, y;
    int i, n;
    float out;
    unsigned char tile;
    float tiles[TILE_AMOUNT*2] = {
        0.2, 0,
        0.5, 1,
        0.7, 2
    };
    for(y=0;y<height;y++){
        for(x=0;x<width;x++){
            tile = 0;
            out = stb_perlin_noise3_seed(x*0.1, y*0.1, 0, 0, 0, 0, seed);
            for(i=0;i<TILE_AMOUNT;i++){
                if(out < tiles[i*2]){
                    tile = tiles[i*2+1];
                    break;
                }
            }
            tilemap[y*width+x] = tile;
        }
    }
    for(i=0;i<hill_num;i++){
        anthills[i].x = (xorshift(&seed)%(width-1))*TILE_W;
        anthills[i].y = (xorshift(&seed)%(height-1))*TILE_H;
        anthills[i].player = !i;
        anthills[i].hp = 100;
        anthills[i].food = 100;
        anthills[i].ants = malloc(ANT_NUM*sizeof(Entity));
        anthills[i].hit = 0;
        if(!anthills[i].ants){
            return 1;
        }
        anthills[i].antnum = ANT_NUM;
        for(n=0;n<ANT_NUM;n++){
            anthills[i].ants[n] = default_ant;
            anthills[i].ants[n].tex = !i ? player :
                (xorshift(&seed)%2 ? enemy1 : enemy2);
            anthills[i].ants[n].x = anthills[i].x+8;
            anthills[i].ants[n].y = anthills[i].y+8;
            anthills[i].ants[n].dx = anthills[i].x+8;
            anthills[i].ants[n].dy = anthills[i].y+8;
            anthills[i].ants[n].walking = 0;
        }
    }
    return 0;
}

void overview_render_anthills(int x, int y, Anthill *anthills, int hill_num,
    Texture *player, Texture *enemy, Texture *player_broken,
    Texture *enemy_broken, Texture *cur, Texture *hit, int cursor) {
    int i, n;
    for(i=0;i<hill_num;i++){
        if(anthills[i].hp){
            if(anthills[i].player){
                gfx_draw_sprite(player, anthills[i].x-x, anthills[i].y-y);
            }else{
                gfx_draw_sprite(enemy, anthills[i].x-x, anthills[i].y-y);
            }
        }else{
            if(anthills[i].player){
                gfx_draw_sprite(player_broken, anthills[i].x-x,
                    anthills[i].y-y);
            }else{
                gfx_draw_sprite(enemy_broken, anthills[i].x-x,
                    anthills[i].y-y);
            }
        }
    }
    for(i=0;i<hill_num;i++){
        for(n=0;n<anthills[i].antnum;n++){
            if(anthills[i].ants[n].visible && anthills[i].ants[n].hit){
                gfx_draw_sprite(hit, anthills[i].ants[n].x-x,
                    anthills[i].ants[n].y-y);
            }
        }
    }
    for(i=0;i<hill_num;i++){
        for(n=0;n<anthills[i].antnum;n++){
            if(anthills[i].ants[n].visible){
                gfx_draw_sprite(anthills[i].ants[n].tex,
                    anthills[i].ants[n].x-x, anthills[i].ants[n].y-y);
            }
        }
    }
    gfx_draw_sprite(cur, anthills[cursor].x-x, anthills[cursor].y-y-32);
}

void overview_update(Anthill *hill, unsigned char *map, unsigned char *used,
    int width, int height, float speed, unsigned int *seed, Audio *food) {
    int i;
    int x, y;
    int sx, sy;
    int min_len;
    int len;
    int tx, ty;
    char found;
    float dist;
    Anthill *target;
    for(i=0;i<hill->antnum;i++){
        sx = 0;
        sy = 0;
        min_len = INT_MAX;
        found = 0;
        switch(hill->ants[i].task){
            case A_FOOD:
                if(!hill->ants[i].walking){
                    for(y=0;y<height;y++){
                        for(x=0;x<width;x++){
                            if(!map[y*width+x] && !used[y*width+x]){
                                len = fabs(x-hill->ants[i].x/TILE_W)+
                                    fabs(y-hill->ants[i].y/TILE_H);
                                if(len < min_len){
                                    min_len = len;
                                    sx = x;
                                    sy = y;
                                    found = 1;
                                }
                            }
                        }
                    }
                    if(!found){
                        hill->ants[i].visible = 0;
                        hill->ants[i].walking = 0;
                        hill->ants[i].task = A_NONE;
                        continue;
                    }
                    used[sy*width+sx] = 1;
                    entity_slide(hill->ants+i, sx*TILE_W+TILE_W/2,
                        sy*TILE_H+TILE_H/2);
                }else if(hill->ants[i].x != hill->ants[i].dx ||
                    hill->ants[i].y != hill->ants[i].dy){
                    entity_update(hill->ants+i, speed);
                }else{
                    tx = hill->ants[i].x/TILE_W;
                    ty = hill->ants[i].y/TILE_H;
                    map[ty*width+tx] = 3;
                    hill->food++;
                    used[ty*width+tx] = 0;
                    entity_slide(hill->ants+i, hill->x+8, hill->y+8);
                    hill->ants[i].task = A_GO_HOME;
#if AUDIO
                    gfx_play_audio(food);
#endif
                }
                break;
            case A_ATTACK:
                if(!hill->ants[i].mission){
                    if(hill->ants[i].x == hill->ants[i].dx &&
                        hill->ants[i].y == hill->ants[i].dy){
                        if(!(xorshift(seed)&0xFF)){
                            entity_slide(hill->ants+i, hill->x+8, hill->y+8);
                            hill->ants[i].task = A_GO_HOME;
                        }else{
                            entity_slide(hill->ants+i,
                                (xorshift(seed)%width)*TILE_W,
                                (xorshift(seed)%height)*TILE_H);
                        }
                    }else{
                        entity_update(hill->ants+i, speed);
                    }
                }else{
                    target = hill->ants[i].target;
                    dist = fabs(target->x-hill->ants[i].x)+
                    fabs(target->y-hill->ants[i].y);
                    if(((int)hill->ants[i].x == (int)hill->ants[i].dx &&
                        (int)hill->ants[i].y == (int)hill->ants[i].dy) ||
                        (dist > HILL_RADIUS && hill->ants[i].ready)){
                        entity_slide(hill->ants+i,
                            target->x+8-HILL_RADIUS/2+
                                xorshift(seed)%HILL_RADIUS,
                            target->y+8-HILL_RADIUS/2+
                                xorshift(seed)%HILL_RADIUS);
                            hill->ants[i].ready = 1;
                    }
                    entity_update(hill->ants+i, speed);
                }
                break;
            case A_GO_HOME:
                if(hill->ants[i].visible){
                    if(hill->ants[i].x != hill->x+8 ||
                        hill->ants[i].y != hill->y+8){
                        entity_update(hill->ants+i, speed);
                    }else{
                        hill->ants[i].visible = 0;
                        hill->ants[i].walking = 0;
                        hill->ants[i].task = A_NONE;
                        hill->ants[i].mission = M_NONE;
                        hill->ants[i].target = NULL;
                        hill->ants[i].ready = 0;
                    }
                }
                break;
            default:
                break;
        }
    }
}

#define MAX(a, b) ((a) > (b) ? (a) : (b))

void overview_tick(unsigned int *seed, unsigned char *map, int width,
    int height, Anthill *hills, int hillnum, Texture *player, Texture *enemy1,
    Texture *enemy2) {
    int x, y, i, n;
    int attacking;
    Stat stat;
    Stat others;
    Anthill *hill;
    for(i=0;i<hillnum;i++){
        hills[i].hit = 0;
        stat = overview_get_stats(hills+i);
        if(!(xorshift(seed)&0x7)){
            if(hills[i].food > 0) hills[i].food--;
        }
        if(hills[i].food == 0){
            if(hills[i].hp > 0) hills[i].hp--;
        }
        if(!hills[i].hp){
            free(hills[i].ants);
            hills[i].ants = NULL;
            hills[i].antnum = 0;
        }
        if(!hills[i].player){
            if(hills[i].hp){
                /* Computer plays */
                if(hills[i].food < (int)(100+(xorshift(seed)&0xF)-8)){
                    overview_assign_task(hills+i, A_FOOD,
                    (xorshift(seed)&0xF)+2);
                }
                if(!(xorshift(seed)&0xF) && stat.attack < 5){
                    overview_assign_task(hills+i, A_ATTACK,
                        (xorshift(seed)&0x3)+2);
                }
                attacking = 0;
                for(n=0;n<hillnum;n++){
                    if(n == i) continue;
                    others = overview_get_stats(hills+n);
                    attacking += others.attack;
                }
                attacking /= hillnum-1;
                if(stat.attack < attacking+(int)((xorshift(seed)&0xF)-8) &&
                    attacking){
                    overview_assign_task(hills+i, A_ATTACK,
                        (xorshift(seed)%attacking)+2);
                }else if(attacking){
                    overview_assign_task(hills+i, A_GO_HOME,
                        (xorshift(seed)%attacking)-attacking/2);
                }
                hill = hills+xorshift(seed)%hillnum;
                if(stat.free > 1){
                    if(!(xorshift(seed)&0x1F) && hill->antnum &&
                        hill != hills+i){
                        overview_attack_hill(hills+i, hill,
                            1+(xorshift(seed)%(stat.free-1)), seed);
                    }
                }
            }
        }
        if(stat.free){
            if(!(xorshift(seed)&MAX(0x3FF/stat.free, 1))){
                /* Add ant */
                overview_add_ant(hills+i, player, enemy1, enemy2, seed);
            }
            if(!(xorshift(seed)&0xF)){
                /* Remove ant */
                if(hills[i].antnum){
                    for(n=0;n<hills[i].antnum;n++){
                        if(hills[i].ants[n].task == A_NONE){
                            overview_remove_ant(hills+i, n);
                            break;
                        }
                    }
                }
            }
            if(!(xorshift(seed)&MAX(0xF/stat.free, 1)) && hills[i].hp < 100){
                /* Add hp to the anthill */
                hills[i].hp++;
            }
        }
        for(n=0;n<hills[i].antnum;n++){
            hills[i].ants[n].hit = 0;
        }
    }
    for(y=0;y<height;y++){
        for(x=0;x<width;x++){
            if(map[y*width+x] == 3 && !(xorshift(seed)&0xFF)){
                map[y*width+x] = 0;
            }
        }
    }
}

void overview_attack(Anthill *hills, int hillnum, unsigned int *seed,
    Audio *attack) {
    Entity *ant;
    Entity *attacking;
    char damaged;
    int i, n, c, l;
    float dist;
    for(i=0;i<hillnum;i++){
        for(n=0;n<hills[i].antnum;n++){
            if(hills[i].ants[n].task == A_ATTACK && hills[i].ants[n].visible){
                for(c=0;c<hillnum;c++){
                    if(c == i) continue;
                    if(!hills[c].hit && hills[c].hp){
                        dist = fabs(hills[c].x-hills[i].ants[n].x)+
                                fabs(hills[c].y-hills[i].ants[n].y);
                        if(dist < ATTACK_DIST){
                            hills[c].hp--;
                            hills[c].hit = 1;
                        }
                    }
                    for(l=0;l<hills[c].antnum;l++){
                        if(hills[c].ants[l].visible){
                            dist = fabs(hills[c].ants[l].x-hills[i].ants[n].x)+
                                fabs(hills[c].ants[l].y-hills[i].ants[n].y);
                            if(dist < ATTACK_DIST){
                                damaged = xorshift(seed)&1;
                                ant = damaged ? hills[i].ants+n :
                                    hills[c].ants+l;
                                attacking = damaged ? hills[c].ants+l :
                                    hills[i].ants+n;
                                if(!ant->hit){
                                    entity_damage(ant,
                                        attacking->force+
                                        xorshift(seed)%(attacking->force ?
                                            attacking->force : 1));
#if AUDIO
                                    gfx_play_audio(attack);
#endif
                                    if(!ant->hp){
                                        overview_remove_ant(damaged ? hills+i :
                                            hills+c, damaged ? n : l);
                                        if(!hills[i].ants || !hills[c].ants){
                                            return;
                                        }
                                    }
                                    ant->hit = 1;
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

void overview_remove_ant(Anthill *anthill, int idx) {
    /*printf("----- Kill %d -----\n", idx);
    puts("-- BEFORE --");
    int i;
    for(i=0;i<anthill->antnum;i++){
        printf("%f, %f ", anthill->ants[i].x, anthill->ants[i].y);
    }
    puts("");*/
    if(anthill->antnum <= 1){
        free(anthill->ants);
        anthill->ants = NULL;
        anthill->antnum = 0;
        anthill->hp = 0;
        return;
    }
    if(anthill->antnum-1 != idx){
        memmove(anthill->ants+idx, anthill->ants+idx+1,
            (anthill->antnum-idx-1)*sizeof(Entity));
    }
    anthill->antnum--;
    /*puts("-- AFTER --");
    for(i=0;i<anthill->antnum;i++){
        printf("%f, %f ", anthill->ants[i].x, anthill->ants[i].y);
    }
    puts("");*/
}

void overview_add_ant(Anthill *anthill, Texture *player, Texture *enemy1,
    Texture *enemy2, unsigned int *seed) {
    Entity *tmp;
    if(!anthill->ants || !anthill->hp || !anthill->antnum) return;
    tmp = realloc(anthill->ants, (anthill->antnum+1)*sizeof(Entity));
    if(!tmp) return;
    anthill->ants = tmp;
    anthill->ants[anthill->antnum] = default_ant;
    anthill->ants[anthill->antnum].tex = anthill->player ? player :
        (xorshift(seed)%2 ? enemy1 : enemy2);
    anthill->ants[anthill->antnum].x = anthill->x+8;
    anthill->ants[anthill->antnum].y = anthill->y+8;
    anthill->ants[anthill->antnum].dx = anthill->x+8;
    anthill->ants[anthill->antnum].dy = anthill->y+8;
    anthill->ants[anthill->antnum].walking = 0;
    anthill->antnum++;
}

void overview_attack_hill(Anthill *src, Anthill *dest, int amount,
    unsigned int *seed) {
    int c = 0;
    int i;
    if(!amount) return;
    for(i=0;i<src->antnum;i++){
        if(!src->ants[i].task){
            src->ants[i].task = A_ATTACK;
            src->ants[i].mission = M_WAR;
            src->ants[i].visible = 1;
            src->ants[i].target = dest;
            src->ants[i].ready = 0;
            entity_slide(src->ants+i,
                dest->x+8+xorshift(seed)%32,
                dest->y+8+xorshift(seed)%32);
            c++;
            if(c >= amount) break;
        }
    }
}

void overview_assign_task(Anthill *hill, char task, int amount) {
    int c = 0;
    int i;
    if(!amount) return;
    for(i=0;i<hill->antnum;i++){
        if(task == A_GO_HOME){
            if(hill->ants[i].task == A_ATTACK){
                hill->ants[i].task = task;
                hill->ants[i].visible = 1;
                entity_slide(hill->ants+i, hill->x+8, hill->y+8);
                c++;
                if(c >= amount) break;
            }
        }else{
            if(!hill->ants[i].task){
                hill->ants[i].task = task;
                hill->ants[i].visible = 1;
                c++;
                if(c >= amount) break;
            }
        }
    }
}

Stat overview_get_stats(Anthill *hill) {
    Stat stat = {0, 0, 0, 0};
    int i;
    for(i=0;i<hill->antnum;i++){
        switch(hill->ants[i].task){
            case A_FOOD:
                stat.food++;
                break;
            case A_ATTACK:
                stat.attack++;
                break;
            case A_GO_HOME:
                stat.go_home++;
                break;
            default:
                stat.free++;
        }
    }
    return stat;
}


