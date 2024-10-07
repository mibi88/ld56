#!/bin/bash

src="src/main.c src/gfx.c src/tilemap.c src/overview.c src/game.c src/entity.c"
audio="assets/food.wav assets/attack.wav"

mkdir -p em

if [ ! -f em/stb ]; then
    git clone https://github.com/nothings/stb em/stb
fi

emcc $src -sUSE_SDL=1 -sUSE_SDL_IMAGE=1 -sLEGACY_GL_EMULATION -sGL_FFP_ONLY \
    -sMODULARIZE=1 -ansi -Wall -Wextra -Wpedantic --use-preload-plugins \
    --preload-file assets/anthill.png \
    --preload-file assets/anthill_broken.png \
    --preload-file assets/anthill_enemy.png \
    --preload-file assets/anthill_enemy_broken.png \
    --preload-file assets/bg.png \
    --preload-file assets/bg_map.png \
    --preload-file assets/cursor.png \
    --preload-file assets/enemy1.png \
    --preload-file assets/enemy2.png \
    --preload-file assets/font.png \
    --preload-file assets/game_over.png \
    --preload-file assets/hit.png \
    --preload-file assets/pause.png \
    --preload-file assets/player.png \
    --preload-file assets/title.png \
    --preload-file assets/tutorial.png \
    --preload-file assets/win.png \
    -o em/index.js -Isrc -Iem

mkdir -p em/assets
cp $audio em/assets

cp src/index.html em
