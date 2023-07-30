#include "phy.h"
#include "global.h"
#include <math.h>

int phy_update(phy_t* phy, map_t* map, int bounce, double height, int elapsed_time) {
  double new_x, new_y, new_z, floor, ceil, time;
  int collision;

  time = elapsed_time / 1000.0;

  phy->velocity.x *= pow(phy->friction, time);
  phy->velocity.y *= pow(phy->friction, time);
  phy->velocity.z -= GRAVITY * time;

  collision = 0;
  new_x = phy->position.x + phy->velocity.x * time;
  new_y = phy->position.y + phy->velocity.y * time;
  new_z = phy->position.z + phy->velocity.z * time;

  floor = height / 2;
  ceil = 1 - height / 2;

  if (new_z < floor) {
    phy->velocity.z = -phy->velocity.z * phy->bouncy;
    collision = 1;
    phy->position.z = floor;
  } else if (new_z > ceil) {
    phy->velocity.z = -phy->velocity.z * phy->bouncy;
    collision = 1;
    phy->position.z = ceil;
  } else {
    phy->position.z = new_z;
  }

  if (!map_get_cell(map, phy->position.x, new_y)) {
    phy->position.y = new_y;
  } else {
    phy->velocity.y = -phy->velocity.y * phy->bouncy;
    new_y = phy->position.y + phy->velocity.y * time;
    phy->position.y = new_y;
    collision = 1;
  }
  if (!map_get_cell(map, new_x, phy->position.y)) {
    phy->position.x = new_x;
  } else {
    phy->velocity.x = -phy->velocity.x * phy->bouncy;
    new_x = phy->position.x + phy->velocity.x * time;
    phy->position.x = new_x;
    collision = 1;
  }
  return collision;
}

void phy_rotate(vec2_t* v, double phi) {
  double dir_x, dir_y;

  dir_x = v->x;
  dir_y = v->y;
  v->x = dir_x * cos(phi) - dir_y * sin(phi);
  v->y = dir_x * sin(phi) + dir_y * cos(phi);
}
