#include <stdio.h>
#include <stdlib.h>

#include <game.h>

unsigned int delta;
float speed;
char rc;
Game game;

#if __EMSCRIPTEN__
#include <emscripten.h>

void loop(void) {
    delta = gfx_frame_ms();
    speed = delta/(float)1000;
    gfx_handle_events();
    rc = game_logic(&game, speed, delta);
    game_draw(&game);
}
#endif

int main(void) {
    gfx_init();
    game_init(&game, 32, 32, 5, 77);
#if __EMSCRIPTEN__
    emscripten_set_main_loop(loop, 0, 0);
#else
    do{
        delta = gfx_frame_ms();
        speed = delta/(float)1000;
        gfx_handle_events();
        rc = game_logic(&game, speed, delta);
        game_draw(&game);
    }while(!gfx_quit_asked() && !rc);
#endif
    game_free(&game);
    gfx_quit();
    return EXIT_SUCCESS;
}

