#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "caster.h"
#include "phy.h"

typedef struct _player_t {
  phy_t    me;
  phy_t    camera_plane;
  uint16_t shot_timer;
  uint8_t  weapon;
  int16_t   health;
  float    shooting;
} player_t;

void player_init(player_t* player);
void player_respawn(player_t* player);
void player_shot_update(player_t* player, sprite_bank_t* sprites);
int  player_local_update(player_t* player, input_t* input, map_t* map, sprite_bank_t* sprites);
void player_cleanup(player_t* player);

#endif
