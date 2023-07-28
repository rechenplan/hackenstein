#ifndef CASTER_H
#define CASTER_H

#include "map.h"
#include "lfb.h"
#include "phy.h"

typedef struct _caster_t {
  phy_t camera;
  phy_t camera_plane;
  lfb_t* lfb;
  double *z_buffer;
} caster_t;

void caster_init(caster_t* cast, lfb_t* lfb, double pos_x, double pos_y, double dir_x, double dir_y);
void caster_cleanup(caster_t* caster);
void caster_draw_map(caster_t* caster, map_t* map);

#endif
