#ifndef PHY_H
#define PHY_H

#include "map.h"

typedef struct _phy_t {
  double pos_x;
  double pos_y;
  double dir_x;
  double dir_y;
} phy_t;

int phy_rel_move(phy_t* phy, map_t* map, double x, double y);
void phy_rotate(phy_t* phy, double phi);

#endif
