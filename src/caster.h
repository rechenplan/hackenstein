#ifndef CASTER_H
#define CASTER_H

#include "map.h"
#include "lfb.h"
#include "physics.h"
#include "player.h"
#include "object.h"

typedef struct _caster_t {
  lfb_t* lfb;
  double* z_buffer;
  int* side;
  int* line_start;
  int* line_end;
} caster_t;

void caster_init(caster_t* cast, lfb_t* lfb);
void caster_cleanup(caster_t* caster);
void caster_update(caster_t* caster, map_t* map, object_bank_t* objects, player_t* player);
void caster_draw_map(caster_t* caster, map_t* map, player_t* player);
void caster_draw_objects(caster_t* caster, object_bank_t* objects, player_t* player);

#endif
