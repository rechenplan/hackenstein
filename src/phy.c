#include "phy.h"
#include <math.h>

void phy_rel_move(phy_t* phy, map_t* map, double x, double y) {
  double new_x, new_y;

  new_x = phy->pos_x + phy->dir_x * y - phy->dir_y * x;
  new_y = phy->pos_y + phy->dir_y * y + phy->dir_x * x;
  if (!map_get_cell(map, phy->pos_x, new_y)) {
    phy->pos_y = new_y;
  }
  if (!map_get_cell(map, new_x, phy->pos_y)) {
    phy->pos_x = new_x;
  }
}

void phy_rotate(phy_t* phy, double phi) {
  double dir_x, dir_y;

  dir_x = phy->dir_x;
  dir_y = phy->dir_y;
  phy->dir_x = dir_x * cos(phi) - dir_y * sin(phi);
  phy->dir_y = dir_x * sin(phi) + dir_y * cos(phi);
}
