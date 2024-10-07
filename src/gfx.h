#ifndef GFX_H
#define GFX_H

#define WIDTH 480
#define HEIGHT 270

enum {
    K_LEFT,
    K_RIGHT,
    K_UP,
    K_DOWN,
    K_SPACE,
    K_ESC,
    K_ENTER,
    K_0,
    K_1,
    K_2,
    K_3,
    K_4,
    K_5,
    K_6,
    K_7,
    K_8,
    K_9,
    K_AMOUNT
};

typedef struct {
    unsigned int id;
    int width;
    int height;
    /* We are using square textures. */
    int real_width;
    int real_height;
} Texture;

#if __EMSCRIPTEN__
typedef struct {
    char *file;
} Audio;
#else
typedef struct {
    void *chunk;
} Audio;
#endif

void gfx_init(void);
char gfx_load_texture(Texture *texture, char *file, int w, int h);
char gfx_load_audio(Audio *audio, char *file);
void gfx_play_audio(Audio *audio);
void gfx_draw_sprite_opt(Texture *texture, float x, float y, float rx,
    float ry, float rw, float rh, float dw, float dh);
#define gfx_draw_sprite(texture, x, y) gfx_draw_sprite_opt(texture, x, y, 0, \
    0, (texture)->width, (texture)->height, (texture)->width, \
    (texture)->height);
void gfx_rect(int x, int y, int w, int h, int r, int g, int b);
void gfx_draw_char(Texture *font, float x, float y, char c);
void gfx_draw_string(Texture *font, float x, float y, char *str);
void gfx_clear(void);
void gfx_update(void);
void gfx_quit(void);
void gfx_handle_events(void);
char gfx_key_pressed(int key);
char gfx_char(void);
char gfx_quit_asked(void);
unsigned int gfx_frame_ms(void);

#endif

