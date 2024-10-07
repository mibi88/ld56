#include <game.h>

#include <overview.h>
#include <tilemap.h>

#include <stdio.h>
#include <stdlib.h>

#include <math.h>

#include <string.h>

#include <limits.h>

void game_init(Game *game, int width, int height, int hills,
    unsigned int seed) {
    if(gfx_load_texture(&game->tiles, "assets/bg_map.png", 64, 64)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->player, "assets/anthill.png", 32, 32)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->enemy, "assets/anthill_enemy.png", 32, 32)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->player_broken, "assets/anthill_broken.png", 32,
        32)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->enemy_broken, "assets/anthill_enemy_broken.png",
        32, 32)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->cur, "assets/cursor.png", 32, 32)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->font, "assets/font.png", 128, 48)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->hit, "assets/hit.png", 16, 16)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->player_ant, "assets/player.png", 16, 16)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->enemy1, "assets/enemy1.png", 16, 16)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->enemy2, "assets/enemy2.png", 16, 16)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->title, "assets/title.png", 480, 270)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->bg, "assets/bg.png", 480, 270)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->pause, "assets/pause.png", 480, 270)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->game_over, "assets/game_over.png", 480, 270)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->win, "assets/win.png", 480, 270)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_texture(&game->tutorial, "assets/tutorial.png", 480, 270)){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_audio(&game->attack, "assets/attack.wav")){
        puts("antwars: Failed to load audio!");
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(gfx_load_audio(&game->food, "assets/food.wav")){
        puts("antwars: Failed to load audio!");
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    game->map = malloc(width*height);
    if(!game->map){
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    game->used = malloc(width*height);
    if(!game->used){
        free(game->map);
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    memset(game->used, 0, width*height);
    game->anthills = malloc(hills*sizeof(Anthill)*2);
    if(!game->anthills){
        free(game->map);
        free(game->used);
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    if(overview_generate(game->map, game->anthills, hills, width, height, seed,
        &game->player_ant, &game->enemy1, &game->enemy2)){
        free(game->map);
        free(game->anthills);
        free(game->used);
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    game->screen = S_TITLE;
    game->w = width;
    game->h = height;
    game->hillnum = hills;
    game->x = 0;
    game->y = 0;
    game->cursor = 0;
    game->lock = 0;
    game->stat = overview_get_stats(game->anthills+game->cursor);
    game->seed = seed;
    game->beforetick = 0;
    game->tutorial_screen = 0;
    strcpy(game->seed_str, "0");
    game->seed_len = strlen(game->seed_str);
}

void game_reset(Game *game, unsigned int seed) {
    if(overview_generate(game->map, game->anthills, game->hillnum, game->w,
        game->h, seed, &game->player_ant, &game->enemy1, &game->enemy2)){
        free(game->map);
        free(game->anthills);
        free(game->used);
        gfx_quit();
        exit(EXIT_FAILURE);
    }
    game->screen = S_TITLE;
    game->x = 0;
    game->y = 0;
    game->cursor = 0;
    game->lock = 0;
    game->stat = overview_get_stats(game->anthills+game->cursor);
    game->seed = seed;
    game->beforetick = 0;
    game->tutorial_screen = 0;
}

#define SPEED 500

char seed_char;

char game_logic(Game *game, float speed, unsigned int delta) {
    float dx, dy;
    float mx, my;
    int n;
    char rc;
    char win;
    seed_char = gfx_char();
    switch(game->screen){
        case S_OVERVIEW:
            game->beforetick += delta;
            while(game->beforetick >= TICK){
                overview_tick(&game->seed, game->map, game->w, game->h,
                    game->anthills, game->hillnum, &game->player_ant,
                    &game->enemy1, &game->enemy2);
                game->beforetick -= TICK;
            }
            game->stat =
                        overview_get_stats(game->anthills+game->cursor);
            if(!game->lock){
                if(gfx_key_pressed(K_LEFT)){
                    game->cursor--;
                    if(game->cursor < 0) game->cursor += game->hillnum;
                    game->stat =
                        overview_get_stats(game->anthills+game->cursor);
                }
                if(gfx_key_pressed(K_RIGHT)){
                    game->cursor++;
                    if(game->cursor >= game->hillnum){
                        game->cursor -= game->hillnum;
                    }
                    game->stat =
                        overview_get_stats(game->anthills+game->cursor);
                }
                if(gfx_key_pressed(K_ESC)){
                    game->screen = S_PAUSE;
                }
                if(gfx_key_pressed(K_ENTER)){
                    game->screen = S_TUTORIAL;
                    game->tutorial_screen = 0;
                }
                if(gfx_key_pressed(K_SPACE)){
                    if(game->anthills[game->cursor].player){
                        game->screen = S_MANAGE;
                        game->item = 0;
                        for(n=0;n<A_AMOUNT;n++) game->adding[n] = 0;
                        game->action = 0;
                        game->stat =
                            overview_get_stats(game->anthills+game->cursor);
                    }else{
                        game->screen = S_DECLARE_WAR;
                        game->num = 0;
                        game->stat =
                            overview_get_stats(game->anthills);
                    }
                }
            }
            game->lock = gfx_key_pressed(K_LEFT) | gfx_key_pressed(K_RIGHT) |
                gfx_key_pressed(K_SPACE) | gfx_key_pressed(K_ESC);
            dx = game->anthills[game->cursor].x-WIDTH/2;
            dy = game->anthills[game->cursor].y-HEIGHT/2;
            mx = fabs((dx-game->x))/100;
            my = fabs((dy-game->y))/100;
            if(game->y > dy){
                game->y-=my*speed*SPEED;
                if(game->y < dy) game->y = dy;
            }
            if(game->y < dy){
                game->y+=my*speed*SPEED;
                if(game->y > dy) game->y = dy;
            }
            if(game->x > dx){
                game->x-=mx*speed*SPEED;
                if(game->x < dx) game->x = dx;
            }
            if(game->x < dx){
                game->x+=mx*speed*SPEED;
                if(game->x > dx) game->x = dx;
            }
            for(n=0;n<game->hillnum;n++){
                overview_update(game->anthills+n, game->map, game->used,
                    game->w, game->h, speed, &game->seed, &game->food);
            }
            overview_attack(game->anthills, game->hillnum, &game->seed,
                &game->attack);
            if(!game->anthills->antnum){
                game->screen = S_GAME_OVER;
            }
            win = 1;
            for(n=1;n<game->hillnum;n++){
                if(game->anthills[n].antnum) win = 0;
            }
            if(win){
                game->screen = S_WIN;
            }
            break;
        case S_MANAGE:
            if(!game->lock){
                if(gfx_key_pressed(K_ESC)){
                    game->screen = S_OVERVIEW;
                }
                if(gfx_key_pressed(K_SPACE)){
                    if(!game->action){
                        game->action = 1;
                    }else{
                        overview_assign_task(game->anthills+game->cursor,
                            game->item, game->adding[game->item]);
                        game->stat =
                            overview_get_stats(game->anthills+game->cursor);
                        game->action = 0;
                    }
                }
                if(gfx_key_pressed(K_UP)){
                    game->item--;
                    if(game->item < 0) game->item += A_AMOUNT;
                }
                if(gfx_key_pressed(K_DOWN)){
                    game->item++;
                    if(game->item >= A_AMOUNT){
                        game->item -= A_AMOUNT;
                    }
                }
                if(gfx_key_pressed(K_LEFT)){
                    game->adding[game->item]--;
                    if(game->adding[game->item] < 0){
                        game->adding[game->item] += game->item == A_GO_HOME ?
                            game->stat.attack+1 : game->stat.free+1;
                    }
                }
                if(gfx_key_pressed(K_RIGHT)){
                    game->adding[game->item]++;
                    if(game->adding[game->item] >= (game->item == A_GO_HOME ?
                        game->stat.attack+1 : game->stat.free+1)){
                        game->adding[game->item] -= game->item == A_GO_HOME ?
                            game->stat.attack+1 : game->stat.free+1;
                    }
                }
            }
            game->lock = gfx_key_pressed(K_SPACE) | gfx_key_pressed(K_UP) |
                gfx_key_pressed(K_DOWN) | gfx_key_pressed(K_ESC) |
                gfx_key_pressed(K_LEFT) | gfx_key_pressed(K_RIGHT);
            break;
        case S_DECLARE_WAR:
            if(!game->lock){
                if(gfx_key_pressed(K_LEFT)){
                    game->num--;
                    if(game->num < 0) game->num += game->stat.free+1;
                }
                if(gfx_key_pressed(K_RIGHT)){
                    game->num++;
                    if(game->num >= game->stat.free+1){
                        game->num -= game->stat.free+1;
                    }
                }
                if(gfx_key_pressed(K_SPACE)){
                    overview_attack_hill(game->anthills,
                        game->anthills+game->cursor, game->num, &game->seed);
                    game->screen = S_OVERVIEW;
                    game->stat =
                        overview_get_stats(game->anthills+game->cursor);
                }
                if(gfx_key_pressed(K_ESC)){
                    game->screen = S_OVERVIEW;
                    game->stat =
                        overview_get_stats(game->anthills+game->cursor);
                }
            }
            game->lock = gfx_key_pressed(K_LEFT) | gfx_key_pressed(K_RIGHT) |
                gfx_key_pressed(K_SPACE) | gfx_key_pressed(K_ESC);
            break;
        case S_PAUSE:
            if(!game->lock){
                if(gfx_key_pressed(K_ENTER)){
                    game->screen = S_TITLE;
                }
                if(gfx_key_pressed(K_ESC)){
                    game->screen = S_OVERVIEW;
                }
            }
            game->lock = gfx_key_pressed(K_ENTER) | gfx_key_pressed(K_ESC);
            break;
        case S_GAME_OVER:
            if(!game->lock){
                if(gfx_key_pressed(K_ENTER)){
                    game->screen = S_TITLE;
                }
            }
            game->lock = gfx_key_pressed(K_ENTER);
            break;
        case S_WIN:
            if(!game->lock){
                if(gfx_key_pressed(K_ENTER)){
                    game->screen = S_TITLE;
                }
            }
            game->lock = gfx_key_pressed(K_ENTER);
            break;
        case S_TITLE:
            if(seed_char == 0x8 && game->seed_len > 0){
                game->seed_str[--game->seed_len] = '\0';
            }else if(seed_char && seed_char != 0x8 &&
                game->seed_len < SEED_MAX){
                game->seed_str[game->seed_len] = seed_char;
                game->seed_len++;
            }
            if(!game->lock){
                if(gfx_key_pressed(K_ENTER)){
                    game_reset(game, atoi(game->seed_str) ?
                        atoi(game->seed_str) : INT_MAX);
                    game->screen = S_TUTORIAL;
                }
                if(gfx_key_pressed(K_ESC)){
                    rc = 1;
                }
            }
            game->lock = gfx_key_pressed(K_ENTER) | gfx_key_pressed(K_ESC);
            break;
        case S_TUTORIAL:
            if(!game->lock){
                if(gfx_key_pressed(K_SPACE)){
                    game->tutorial_screen++;
                    if(game->tutorial_screen >= TUTORIAL_NUM){
                        game->screen = S_OVERVIEW;
                    }
                }
                if(gfx_key_pressed(K_ESC)){
                    game->screen = S_OVERVIEW;
                }
            }
            game->lock = gfx_key_pressed(K_SPACE) | gfx_key_pressed(K_ESC);
            break;
        default:
            game->screen = S_TITLE;
    }
    return rc;
}

void game_draw(Game *game) {
    char buffer[200];
    gfx_clear();
    switch(game->screen){
        case S_OVERVIEW:
            tilemap_render((int)game->x, (int)game->y, &game->tiles, game->map,
                game->w, game->h);
            overview_render_anthills((int)game->x, (int)game->y,
                game->anthills, game->hillnum, &game->player, &game->enemy,
                &game->player_broken, &game->enemy_broken, &game->cur,
                &game->hit, game->cursor);
            gfx_rect(0, 0, WIDTH, 24, 255, 255, 255);
            gfx_rect(0, 24, WIDTH, 4, 30, 30, 30);
            if(game->anthills[game->cursor].player){
                sprintf(buffer, "Your anthill - Ants: %d - HP: %d - Food: %d\n"
                    "[SPACE] Manage [ENTER] Help",
                    game->anthills[game->cursor].antnum,
                    game->anthills[game->cursor].hp,
                    game->anthills[game->cursor].food);
            }else{
                sprintf(buffer, "Enemy anthill - Ants: %d - HP: %d\n"
                    "[SPACE] Attack [ENTER] Help",
                    game->anthills[game->cursor].antnum,
                    game->anthills[game->cursor].hp);
            }
            gfx_draw_string(&game->font, 5, 5, buffer);
            break;
        case S_MANAGE:
            if(!game->anthills[game->cursor].player){
                game->screen = S_OVERVIEW;
                break;
            }
            gfx_draw_sprite(&game->bg, 0, 0);
            gfx_draw_string(&game->font, 32, 5, "--- Your anthill ---");
            sprintf(buffer, "Ants: %d", game->anthills[game->cursor].antnum);
            gfx_draw_string(&game->font, 32, 5+8, buffer);
            sprintf(buffer, "Food: %d", game->anthills[game->cursor].food);
            gfx_draw_string(&game->font, 32, 5+16, buffer);
            sprintf(buffer, "Free: %d", game->stat.free);
            gfx_draw_string(&game->font, 32, 5+24+8, buffer);
            sprintf(buffer, "Searching food: %d", game->stat.food);
            gfx_draw_string(&game->font, 32, 5+24+8*2, buffer);
            sprintf(buffer, "Attacking: %d", game->stat.attack);
            gfx_draw_string(&game->font, 32, 5+24+8*3, buffer);
            sprintf(buffer, "Going home: %d", game->stat.go_home);
            gfx_draw_string(&game->font, 32, 5+24+8*4, buffer);
            gfx_draw_string(&game->font, 32, 5+24+8*6, "[ESC] Return to map");
            gfx_draw_string(&game->font, 16, 5+32+8*game->item, ">");
            if(game->action){
                if(game->item){
                    sprintf(buffer, "Add < %d > ants to the task",
                        game->adding[game->item]);
                    gfx_draw_string(&game->font, 200, 5+32+8*game->item,
                        buffer);
                }
                gfx_draw_string(&game->font, 32, 5+24+8*7,
                    "[SPACE] Validate action");
            }
            break;
        case S_DECLARE_WAR:
            gfx_draw_sprite(&game->bg, 0, 0);
            gfx_draw_string(&game->font, 32, 5, "--- Attack! ---");
            sprintf(buffer, "Ants: %d", game->anthills->antnum);
            gfx_draw_string(&game->font, 32, 5+8, buffer);
            sprintf(buffer, "Free: %d", game->stat.free);
            gfx_draw_string(&game->font, 32, 5+16, buffer);
            sprintf(buffer, "Going to war: < %d >", game->num);
            gfx_draw_string(&game->font, 32, 5+24+8, buffer);
            gfx_draw_string(&game->font, 32, 5+48, "[ESC] Return to map");
            if(game->num){
                gfx_draw_string(&game->font, 32, 5+56, "[SPACE] Send troops");
            }
            break;
        case S_PAUSE:
            gfx_draw_sprite(&game->pause, 0, 0);
            gfx_draw_string(&game->font, 32, HEIGHT-40-5, "[ESC] Continue");
            gfx_draw_string(&game->font, 32, HEIGHT-32-5,
                "[ENTER] Go to title");
            break;
        case S_GAME_OVER:
            gfx_draw_sprite(&game->game_over, 0, 0);
            gfx_draw_string(&game->font, 32, HEIGHT-32-5,
                "[ENTER] Go to title");
            break;
        case S_WIN:
            gfx_draw_sprite(&game->win, 0, 0);
            gfx_draw_string(&game->font, 32, HEIGHT-32-5,
                "[ENTER] Go to title");
            break;
        case S_TITLE:
            gfx_draw_sprite(&game->title, 0, 0);
            gfx_draw_string(&game->font,
                WIDTH/2-(strlen("[ENTER]")*8)/2, HEIGHT-88-5,
                "[ENTER]");
            gfx_draw_string(&game->font,
                WIDTH/2-(strlen("Start")*8)/2, HEIGHT-80-5,
                "Start");
            gfx_draw_string(&game->font,
                WIDTH/2-(strlen("game")*8)/2, HEIGHT-72-5,
                "game");
            sprintf(buffer, "Seed: %s", game->seed_str);
            gfx_draw_string(&game->font, 32, HEIGHT/2-8, buffer);
            gfx_draw_string(&game->font, 32+48+8*game->seed_len, HEIGHT/2,
                "^");
            gfx_draw_string(&game->font, WIDTH-9*8-32, HEIGHT/2, "by Mibi88");
            gfx_draw_string(&game->font, WIDTH-7*8-32, HEIGHT/2-8, "v.1.0.1");
            break;
        case S_TUTORIAL:
            gfx_draw_sprite(&game->tutorial, 0, 0);
            switch(game->tutorial_screen){
                case 0:
                    gfx_draw_string(&game->font, 8+8, 12+8,
                        "Welcome to the world of ants.\n"
                        "You are a queen ant.\n\n"
                        "Your goal is to destroy the\n"
                        "enemy anthills (which are\n"
                        "darker than yours).\n\n"
                        "By pressing space while\n"
                        "having your anthill selected\n"
                        "(it's shown in the status bar),\n"
                        "you can access an anthill\n"
                        "[SPACE] Go to the next screen.");
                    break;
                case 1:
                    gfx_draw_string(&game->font, 8+8, 12+8,
                        "management screen, where you\n"
                        "assign tasks to your ants:\n"
                        "- Food: Gather food for the\n"
                        "  anthill. If your anthill\n"
                        "  has no more food, it will\n"
                        "  get damaged (loose HP).\n"
                        "- Attack: Let your ants walk\n"
                        "  trought the world and kill\n"
                        "  the enemy ants.\n"
                        "- Go home: Ask fighting ants\n"
                        "  to come back home. It also\n"
                        "[SPACE] Go to the next screen.");
                    break;
                case 2:
                    gfx_draw_string(&game->font, 8+8, 12+8,
                        "  asks ants attacking a hill\n"
                        "  to come back (I'll explain\n"
                        "  how you can attack hills\n"
                        "  next).\n"
                        "To assign a task, choose\n"
                        "the task with the up and\n"
                        "down keys, then press space,\n"
                        "which opens a menu where\n"
                        "you can select the number\n"
                        "of ants you want to assign\n"
                        "to the task with the left\n"
                        "[SPACE] Go to the next screen.");
                        
                    break;
                case 3:
                    gfx_draw_string(&game->font, 8+8, 12+8,
                        "and right arrow keys.\n"
                        "Validate your action by\n"
                        "pressing space.\n"
                        "You can exit this screen\n"
                        "By pressing the escape key.\n"
                        "To attack hills, use the\n"
                        "left and right keys to\n"
                        "select the hill you want\n"
                        "to attack. Then press\n"
                        "space to access a screen\n"
                        "where you can choose how\n"
                        "[SPACE] Go to the next screen.");
                    break;
                case 4:
                    gfx_draw_string(&game->font, 8+8, 12+8,
                        "many ants you want to\n"
                        "send to the hill with the\n"
                        "left and right arrow keys.\n"
                        "Validate your choice with\n"
                        "space or press space to.\n"
                        "abort your action.\n"
                        "Ps: Your ants are the red\n"
                        "ants.\n"
                        "You can reopen the help\n"
                        "by pressing enter from\n"
                        "the map view\n"
                        "[SPACE] Go to the next screen.");
                    break;
                case 5:
                    gfx_draw_string(&game->font, 8+8, 12+8,
                        "Have fun!\n"
                        "Mibi88\n\n"
                        "This game was made for\n"
                        "the Ludum Dare 56 compo\n"
                        "[SPACE] Close help.");
                    break;
            }
            gfx_draw_string(&game->font, 325, HEIGHT-32,
                "[ESC] Skip tutorial");
            break;
        default:
            game->screen = S_TITLE;
    }
    gfx_update();
}

void game_free(Game *game) {
    free(game->map);
    free(game->anthills);
    free(game->used);
}

