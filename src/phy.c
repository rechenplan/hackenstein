#include "phy.h"
#include "global.h"
#include <math.h>

int phy_update(phy_t* phy, map_t* map, int bounce, double height, int elapsed_time) {
  double new_x, new_y, new_z, floor, ceil, time;
  int collision;

  time = elapsed_time / 1000.0;

  phy->vel_x *= pow(phy->friction, time);
  phy->vel_y *= pow(phy->friction, time);
  phy->vel_z -= GRAVITY * time;

  collision = 0;
  new_x = phy->pos_x + phy->vel_x * time;
  new_y = phy->pos_y + phy->vel_y * time;
  new_z = phy->pos_z + phy->vel_z * time;

  floor = height / 2;
  ceil = 1 - height / 2;

  if (new_z < floor) {
    if (bounce) {
      phy->vel_z = -phy->vel_z * phy->bouncy;
    }
    collision = 1;
    phy->pos_z = floor;
  } else if (new_z > ceil) {
    if (bounce) {
      phy->vel_z = -phy->vel_z * phy->bouncy;
    }
    collision = 1;
    phy->pos_z = ceil;
  } else {
    phy->pos_z = new_z;
  }

  if (!map_get_cell(map, phy->pos_x, new_y)) {
    phy->pos_y = new_y;
  } else {
    if (bounce) {
      phy->vel_y = -phy->vel_y * phy->bouncy;
      new_y = phy->pos_y + phy->vel_y * time;
      phy->pos_y = new_y;
    }
    collision = 1;
  }
  if (!map_get_cell(map, new_x, phy->pos_y)) {
    phy->pos_x = new_x;
  } else {
    if (bounce) {
      phy->vel_x = -phy->vel_x * phy->bouncy;
      new_x = phy->pos_x + phy->vel_x * time;
      phy->pos_x = new_x;
    }
    collision = 1;
  }
  return collision;
}

void phy_rotate(phy_t* phy, double phi) {
  double dir_x, dir_y;

  dir_x = phy->dir_x;
  dir_y = phy->dir_y;
  phy->dir_x = dir_x * cos(phi) - dir_y * sin(phi);
  phy->dir_y = dir_x * sin(phi) + dir_y * cos(phi);
}
