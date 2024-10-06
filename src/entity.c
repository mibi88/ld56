#include <entity.h>
#include <math.h>

#include <float.h>

#define SPEED 50

float entity_dist(Entity *entity, float dx, float dy) {
    return sqrt((dx-entity->x)*(dx-entity->x)+(dy-entity->y)*(dy-entity->y));
}

void entity_slide(Entity *entity, float dx, float dy) {
    float len;
    entity->dx = dx;
    entity->dy = dy;
    entity->walking = 1;
    /*printf("%f, %f, %f, %f\n", entity->x, entity->y, dx, dy);*/
    len = sqrt((dx-entity->x)*(dx-entity->x)+(dy-entity->y)*(dy-entity->y));
    entity->vx = (dx-entity->x)/len;
    entity->vy = (dy-entity->y)/len;
}

void entity_update(Entity *entity, float speed) {
    if(entity->walking){
        entity->x += entity->vx*SPEED*speed;
        entity->y += entity->vy*SPEED*speed;
        if((entity->vx > 0 && entity->x > entity->dx) ||
            (entity->vx < 0 && entity->x < entity->dx)){
            entity->vx = 0;
            entity->x = entity->dx;
        }
        if((entity->vy > 0 && entity->y > entity->dy) ||
            (entity->vy < 0 && entity->y < entity->dy)){
            entity->vy = 0;
            entity->y = entity->dy;
        }
    }
}

void entity_damage(Entity *entity, int damage) {
    entity->hp -= damage;
    if(entity->hp < 0) entity->hp = 0;
    if(!entity->hp){
        entity->visible = 0;
        entity->force = 0;
        entity->task = A_NONE;
    }
}

