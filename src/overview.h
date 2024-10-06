#ifndef OVERVIEW_H
#define OVERVIEW_H

#include <tilemap.h>
#include <gfx.h>
#include <entity.h>

#define AUDIO 1

typedef struct {
    int x, y;
    int hp;
    Entity *ants;
    int antnum;
    int food;
    char player;
    char hit;
} Anthill;

typedef struct {
    int free;
    int food;
    int attack;
    int go_home;
} Stat;

int overview_generate(unsigned char *tilemap, Anthill *anthills, int hill_num,
    int width, int height, unsigned int seed, Texture *player, Texture *enemy1,
    Texture *enemy2);
void overview_render_anthills(int x, int y, Anthill *anthills, int hill_num,
    Texture *player, Texture *enemy, Texture *player_broken,
    Texture *enemy_broken, Texture *cur, Texture *hit, int cursor);
void overview_update(Anthill *hill, unsigned char *map, unsigned char *used,
    int width, int height, float speed, unsigned int *seed, Audio *food);
void overview_tick(unsigned int *seed, unsigned char *map, int width,
    int height, Anthill *hills, int hillnum, Texture *player, Texture *enemy1,
    Texture *enemy2);
void overview_attack(Anthill *hills, int hillnum, unsigned int *seed,
    Audio *attack);
void overview_remove_ant(Anthill *anthill, int idx);
void overview_add_ant(Anthill *anthill, Texture *player, Texture *enemy1,
    Texture *enemy2, unsigned int *seed);
void overview_attack_hill(Anthill *src, Anthill *dest, int amount,
    unsigned int *seed);
void overview_assign_task(Anthill *hill, char task, int amount);
Stat overview_get_stats(Anthill *hill);

#endif

