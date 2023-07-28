#include "phy.h"
#include <math.h>

void phy_rel_move(phy_t* phy, double x, double y) {
  phy->pos_x += phy->dir_x * y;
  phy->pos_y += phy->dir_y * y;
  phy->pos_x -= phy->dir_y * x;
  phy->pos_y += phy->dir_x * x;
}

void phy_rotate(phy_t* phy, double phi) {
  double dir_x, dir_y;

  dir_x = phy->dir_x;
  dir_y = phy->dir_y;
  phy->dir_x = dir_x * cos(phi) - dir_y * sin(phi);
  phy->dir_y = dir_x * sin(phi) + dir_y * cos(phi);
}

