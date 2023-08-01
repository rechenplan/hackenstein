#include "phy.h"
#include "global.h"
#include <math.h>

int phy_update(phy_t* physical, map_t* map, int bounce, double height, int elapsed_time) {
  double new_x, new_y, new_z, floor, ceil, time;
  int collision;

  time = elapsed_time / 1000.0;

  collision = 0;
  new_x = physical->position.x + physical->velocity.x * time;
  new_y = physical->position.y + physical->velocity.y * time;
  new_z = physical->position.z + physical->velocity.z * time;

  floor = height / 2;
  ceil = 1 - height / 2;

  if (new_z < floor) {
    physical->velocity.z = -physical->velocity.z * physical->bouncy;
    collision = 1;
    physical->position.z = floor;
  } else if (new_z > ceil) {
    physical->velocity.z = -physical->velocity.z * physical->bouncy;
    collision = 1;
    physical->position.z = ceil;
  } else {
    physical->position.z = new_z;
  }

  if (!map_get_cell(map, physical->position.x, new_y)) {
    physical->position.y = new_y;
  } else {
    physical->velocity.y = -physical->velocity.y * physical->bouncy;
    new_y = physical->position.y + physical->velocity.y * time;
    physical->position.y = new_y;
    collision = 1;
  }
  if (!map_get_cell(map, new_x, physical->position.y)) {
    physical->position.x = new_x;
  } else {
    physical->velocity.x = -physical->velocity.x * physical->bouncy;
    new_x = physical->position.x + physical->velocity.x * time;
    physical->position.x = new_x;
    collision = 1;
  }

  physical->velocity.x *= pow(physical->friction, time);
  physical->velocity.y *= pow(physical->friction, time);
  physical->velocity.z -= GRAVITY * time;

  return collision;
}

void phy_rotate(vec2_t* v, double rotation) {
  double dir_x, dir_y;

  dir_x = v->x;
  dir_y = v->y;
  v->x = dir_x * cos(rotation) - dir_y * sin(rotation);
  v->y = dir_x * sin(rotation) + dir_y * cos(rotation);
}
