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

typedef struct _phy_t {
  vec3_t position;
  vec3_t velocity;
  vec2_t direction;
  float friction;
  float bouncy;
} phy_t;

int phy_update(phy_t* phy, map_t* map, int bounce, double height, int elapsed_time);
void phy_rotate(vec2_t* v, double phi);

#endif
