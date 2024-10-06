#ifndef ENTITY_H
#define ENTITY_H

#include <gfx.h>

enum {
    A_NONE,
    A_FOOD,
    A_ATTACK,
    A_GO_HOME,
    A_AMOUNT
};

enum {
    T_ANT,
    T_PANGOLIN,
    T_AMOUNT
};

enum {
    M_NONE,
    M_WAR,
    M_AMOUNT
};

typedef struct {
    float x, y;
    float vx, vy;
    float dx, dy;
    char walking;
    char task;
    char visible;
    char type;
    char hit;
    int hp;
    int force;
    char mission;
    void *target;
    char ready;
    Texture *tex;
} Entity;

float entity_dist(Entity *entity, float dx, float dy);
void entity_slide(Entity *entity, float dx, float dy);
void entity_update(Entity *entity, float speed);
void entity_damage(Entity *entity, int damage);

#endif

