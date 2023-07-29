#include "phy.h"
#include <math.h>

int phy_rel_move(phy_t* phy, map_t* map, double x, double y, int bounce, double height) {
  double new_x, new_y, new_z, floor, ceil;
  int collision;

  collision = 0;
  new_x = phy->pos_x + phy->vel_x * y - phy->vel_y * x;
  new_y = phy->pos_y + phy->vel_y * y + phy->vel_x * x;
  new_z = phy->pos_z + phy->vel_z;

  floor = height / 2;
  ceil = 1 - height / 2;

  if (new_z < floor) {
    if (bounce) {
      phy->vel_z = -phy->vel_z * 0.6;
    }
    collision = 1;
    phy->pos_z = floor;
  } else if (new_z > ceil) {
    if (bounce) {
      phy->vel_z = -phy->vel_z * 0.6;
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
      phy->vel_y = -phy->vel_y;
      new_y = phy->pos_y + phy->vel_y * y + phy->vel_x * x;
      phy->pos_y = new_y;
    }
    collision = 1;
  }
  if (!map_get_cell(map, new_x, phy->pos_y)) {
    phy->pos_x = new_x;
  } else {
    if (bounce) {
      phy->vel_x = -phy->vel_x;
      new_x = phy->pos_x + phy->vel_x * y - phy->vel_y * x;
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
