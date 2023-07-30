#ifndef CASTER_H
#define CASTER_H

#include "map.h"
#include "lfb.h"
#include "phy.h"
#include "sprite.h"

typedef struct _caster_t {
  lfb_t* lfb;
  double *z_buffer;
} caster_t;

void caster_init(caster_t* cast, lfb_t* lfb);
void caster_cleanup(caster_t* caster);
void caster_update(caster_t* caster, map_t* map, sprite_bank_t* sprites, phy_t* camera, phy_t* camera_plane, int my_sprite);
void caster_draw_map(caster_t* caster, map_t* map, phy_t* camera, phy_t* camera_plane);
void caster_draw_sprites(caster_t* caster, sprite_bank_t* sprites, phy_t* camera, phy_t* camera_plane, int my_sprite);

#endif
