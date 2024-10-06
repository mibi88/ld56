#include <gfx.h>
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_mixer.h>
#include <GL/gl.h>
#include <GL/glu.h>

char _keys[K_AMOUNT];
char _quit_asked;
unsigned int _ms = 0;
unsigned int _delta = 0;

SDLKey _sdl_keys[K_AMOUNT] = {
    SDLK_LEFT,
    SDLK_RIGHT,
    SDLK_UP,
    SDLK_DOWN,
    SDLK_SPACE,
    SDLK_ESCAPE,
    SDLK_RETURN,
    SDLK_0,
    SDLK_1,
    SDLK_2,
    SDLK_3,
    SDLK_4,
    SDLK_5,
    SDLK_6,
    SDLK_7,
    SDLK_8,
    SDLK_9,
};

char _key_char;

void gfx_init(void) {
    const SDL_VideoInfo *info;
    int bpp;
    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO)){
        fputs("antwars: SDL_Init failed!\n", stderr);
        exit(EXIT_FAILURE);
    }
    
    info = SDL_GetVideoInfo();
    if(!info){
        fputs("antwars: SDL_GetVideoInfo failed!\n", stderr);
        exit(EXIT_FAILURE);
    }
    
    bpp = info->vfmt->BitsPerPixel;
    
    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 0);
    
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    
    SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 1);
    
    if(!SDL_SetVideoMode(1920, 1080, bpp, SDL_OPENGL | SDL_FULLSCREEN)){
        fputs("antwars: SDL_SetVideoMode failed!\n", stderr);
        exit(EXIT_FAILURE);
    }
    
    if(Mix_OpenAudio(44100, AUDIO_U8, 2, 2048) < 0){
        fputs("antwars: SDL_OpenAudio failed!\n", stderr);
        exit(EXIT_FAILURE);
    }
    
    glClearColor(0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_FLAT);
    glViewport(0, 0, 1920, 1080);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluOrtho2D(0, WIDTH, HEIGHT, 0);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_ALPHA_TEST);
    glAlphaFunc(GL_GREATER, 0.5);
    
    memset(_keys, 0, K_AMOUNT);
    
    _key_char = '\0';
    
    _ms = SDL_GetTicks();
}

char gfx_load_texture(Texture *texture, char *file, int w, int h) {
    SDL_Surface *image;
    unsigned int id = 0;
    image = IMG_Load(file);
    if(!image){
        fputs("antwars: IMG_Load failed!\n", stderr);
        texture->id = 0;
        texture->real_width = 0;
        texture->real_height = 0;
        texture->width = 0;
        texture->height = 0;
        return 1;
    }
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image->w, image->h, 0, GL_RGBA,
        GL_UNSIGNED_BYTE, image->pixels);
    
    SDL_FreeSurface(image);
    
    
    texture->real_width = image->w;
    texture->real_height = image->h;
    texture->width = w;
    texture->height = h;
    
    texture->id = id;
    return 0;
}

char gfx_load_audio(Audio *audio, char *file) {
    Mix_Chunk *chunk;
    chunk = Mix_LoadWAV(file);
    audio->chunk = chunk;
    if(!chunk) return 1;
    return 0;
}

void gfx_play_audio(Audio *audio) {
    if(!audio->chunk) return;
    Mix_PlayChannel(-1, audio->chunk, 0);
}

void gfx_draw_sprite_opt(Texture *texture, float x, float y, float rx,
    float ry, float rw, float rh, float dw, float dh) {
    float uv[4];
    glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, texture->id);
    uv[0] = texture->width/(float)texture->real_width/(float)texture->width*rx;
    uv[1] = texture->width/(float)texture->real_width/(float)texture->width*ry;
    
    uv[2] = texture->width/(float)texture->real_width/(float)texture->width*
        (rx+rw);
    uv[3] = texture->width/(float)texture->real_width/(float)texture->width*
        (ry+rh);
    glBegin(GL_QUADS);
    glTexCoord2f(uv[0], uv[1]);
    /* Cast X and Y to int for pixel perfect pixel art. */
    glVertex2i((int)x, (int)y);
    glTexCoord2f(uv[0], uv[3]);
    glVertex2i((int)x, (int)y+dh);
    glTexCoord2f(uv[2], uv[3]);
    glVertex2i((int)x+dw, (int)y+dh);
    glTexCoord2f(uv[2], uv[1]);
    glVertex2i((int)x+dw, (int)y);
    glEnd();
    glDisable(GL_TEXTURE_2D);
}

void gfx_rect(int x, int y, int w, int h, int r, int g, int b) {
    glBindTexture(GL_TEXTURE_2D, 0);
    glDisable(GL_TEXTURE_2D);
    glColor3f(r/255.0, g/255.0, b/255.0);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x, y+h);
    glVertex2i(x+w, y+h);
    glVertex2i(x+w, y);
    glEnd();
    glColor3f(1, 1, 1);
}

void gfx_draw_char(Texture *font, float x, float y, char c) {
    int px, py;
    int w = font->width>>4, h = font->height/6;
    c -= 0x20;
    px = c&0xF;
    py = c>>4;
    gfx_draw_sprite_opt(font, x, y, px*w, py*h, w, h, w, h);
}

void gfx_draw_string(Texture *font, float x, float y, char *str) {
    size_t i;
    int w = font->width/16, h = font->height/6;
    int sx = x;
    for(i=0;i<strlen(str);i++){
        if(str[i] == '\n'){
            y += h;
            sx = x;
            continue;
        }
        gfx_draw_char(font, sx, y, str[i]);
        sx += w;
    }
}

void gfx_clear(void) {
    glClear(GL_COLOR_BUFFER_BIT);
}


void gfx_update(void) {
    glFlush();
    SDL_GL_SwapBuffers();
}

void gfx_quit(void) {
    SDL_Quit();
}

void gfx_handle_events(void) {
    SDL_Event event;
    size_t i;
    _quit_asked = 0;
    _key_char = '\0';
    while(SDL_PollEvent(&event)){
        switch(event.type){
            case SDL_QUIT:
                _quit_asked = 1;
                break;
            case SDL_KEYDOWN:
                if(event.key.keysym.sym >= SDLK_0 &&
                    event.key.keysym.sym <= SDLK_9){
                    _key_char = event.key.keysym.sym-SDLK_0+'0';
                }
                if(event.key.keysym.sym == SDLK_BACKSPACE){
                    _key_char = 0x8;
                }
                for(i=0;i<K_AMOUNT;i++){
                    if(_sdl_keys[i] == event.key.keysym.sym){
                        _keys[i] = 1;
                        break;
                    }
                }
                break;
            case SDL_KEYUP:
                for(i=0;i<K_AMOUNT;i++){
                    if(_sdl_keys[i] == event.key.keysym.sym){
                        _keys[i] = 0;
                        break;
                    }
                }
                break;
        }
    }
    /*while((_delta = SDL_GetTicks()-_ms) < 20);*/
    _delta = SDL_GetTicks()-_ms;
    _ms = SDL_GetTicks();
}

char gfx_key_pressed(int key) {
    if(key < 0 || key >= K_AMOUNT) return 0;
    return _keys[key];
}

char gfx_char(void) {
    return _key_char;
}

char gfx_quit_asked(void) {
    return _quit_asked;
}

unsigned int gfx_frame_ms(void) {
    return _delta;
}

