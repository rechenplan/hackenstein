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
  float pos_x;
  float pos_y;
  float pos_z;
  float vel_x;
  float vel_y;
  float vel_z;
  float dir_x;
  float dir_y;
  float friction;
  float bouncy;
} phy_t;

int phy_update(phy_t* phy, map_t* map, int bounce, double height, int elapsed_time);
void phy_rotate(phy_t* phy, double phi);

#endif
