#ifndef PHY_H
#define PHY_H

#include "map.h"

typedef struct _vec3_t {
  float x;
  float y;
  float z;
} vec3_t;

typedef struct _vec2_t {
  float x;
  float y;
} vec2_t;

typedef struct _physics_t {
  vec3_t position;
  vec3_t velocity;
  float rotation;
  float friction;
  float bouncy;
} physics_t;

int physics_update(physics_t* phy, map_t* map, int bounce, double height, int elapsed_time);
void physics_rotate(vec2_t* v, double rotation);

#endif
