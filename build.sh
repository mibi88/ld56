#!/bin/bash

src="src/main.c src/gfx.c src/tilemap.c src/overview.c src/game.c src/entity.c"

cc $src -o main -ansi -lSDL -lSDL_image -lSDL_mixer -lGL -lGLU -lm -Isrc \
    -Wall -Wextra -Wpedantic -g
strip main

