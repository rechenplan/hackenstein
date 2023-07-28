#ifndef CASTER_H
#define CASTER_H

#include "map.h"
#include "lfb.h"

typedef struct _caster_t {
  double pos_x;
  double pos_y;
  double dir_x;
  double dir_y;
  double plane_x;
  double plane_y;
  lfb_t* lfb;
  double *z_buffer;
} caster_t;

void caster_init(caster_t* cast, lfb_t* lfb, double pos_x, double pos_y, double dir_x, double dir_y);
void caster_cleanup(caster_t* caster);
void caster_draw_map(caster_t* caster, map_t* map);

#endif
