#ifndef GAME_H
#define GAME_H

#include <gfx.h>

#include <overview.h>

#define TICK 500

#define SEED_MAX 6

#define TUTORIAL_NUM 6

enum {
    S_TITLE,
    S_TUTORIAL,
    S_OVERVIEW,
    S_MANAGE,
    S_DECLARE_WAR,
    S_GAME_OVER,
    S_WIN,
    S_PAUSE,
    S_AMOUNT
};

typedef struct {
    Texture tiles, player, enemy, player_broken, enemy_broken, cur, font;
    Texture player_ant, enemy1, enemy2, hit, title, bg, pause, win, game_over;
    Texture tutorial;
    Audio attack, food;
    unsigned char *map;
    unsigned char *used;
    Anthill *anthills;
    int w, h;
    int hillnum;
    float x, y;
    int cursor;
    char lock;
    char screen;
    Stat stat;
    int adding[A_AMOUNT];
    char action;
    int item;
    int num;
    unsigned int seed;
    unsigned int beforetick;
    char seed_str[SEED_MAX+1];
    int seed_len;
    int tutorial_screen;
} Game;

void game_init(Game *game, int width, int height, int hills,
    unsigned int seed);
void game_reset(Game *game, unsigned int seed);
char game_logic(Game *game, float speed, unsigned int delta);
void game_draw(Game *game);
void game_free(Game *game);

#endif

