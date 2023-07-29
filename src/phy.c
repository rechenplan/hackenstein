#include "phy.h"
#include <math.h>

int phy_rel_move(phy_t* phy, map_t* map, double x, double y, int bounce) {
  double new_x, new_y, phi;
  int collision;

  collision = 0;
  new_x = phy->pos_x + phy->dir_x * y - phy->dir_y * x;
  new_y = phy->pos_y + phy->dir_y * y + phy->dir_x * x;
  if (!map_get_cell(map, phy->pos_x, new_y)) {
    phy->pos_y = new_y;
  } else {
    if (bounce) {
      phy->dir_y = -phy->dir_y;
      new_y = phy->pos_y + phy->dir_y * y + phy->dir_x * x;
      phy->pos_y = new_y;
    }
    collision = 1;
  }
  if (!map_get_cell(map, new_x, phy->pos_y)) {
    phy->pos_x = new_x;
  } else {
    if (bounce) {
      phy->dir_x = -phy->dir_x;
      new_x = phy->pos_x + phy->dir_x * y - phy->dir_y * x;
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
