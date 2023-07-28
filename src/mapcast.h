#ifndef MAPCAST_H
#define MAPCAST_H

#include "map.h"
#include "lfb.h"

typedef struct _camera_t {
  double pos_x;
  double pos_y;
  double dir_x;
  double dir_y;
  double plane_x;
  double plane_y;
} camera_t;

camera_t mapcast_create_camera(double pos_x, double pos_y, double dir_x, double dir_y);
void mapcast_draw_map(map_t* map, lfb_t* lfb, camera_t* cam);

#endif