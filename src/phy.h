#ifndef PHY_H
#define PHY_H

#include "map.h"

typedef struct _phy_t {
  double pos_x;
  double pos_y;
  double pos_z;

  double dir_x;
  double dir_y;

  double vel_x;
  double vel_y;
  double vel_z;
} phy_t;

int phy_rel_move(phy_t* phy, map_t* map, double x, double y, int bounce);
void phy_rotate(phy_t* phy, double phi);

#endif
