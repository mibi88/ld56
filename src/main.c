#include <stdio.h>
#include <stdlib.h>

#include <game.h>

int main(void) {
    Game game;
    unsigned int delta;
    float speed;
    char rc;
    gfx_init();
    game_init(&game, 32, 32, 5, 77);
    do{
        delta = gfx_frame_ms();
        speed = delta/(float)1000;
        gfx_handle_events();
        rc = game_logic(&game, speed, delta);
        game_draw(&game);
    }while(!gfx_quit_asked() && !rc);
    game_free(&game);
    gfx_quit();
    return EXIT_SUCCESS;
}

