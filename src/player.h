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
  uint8_t  id;
  uint8_t  shot_timer;
  uint8_t  weapon;
  int8_t   health;
  float    shooting;

  int      sprite;

} player_t;

void player_init(player_t* player);
void player_respawn(player_t* player);
void player_update(player_t* player, sprite_bank_t* sprites, map_t* map);
int  player_process_input(player_t* player, input_t* input);
void player_cleanup(player_t* player);

#endif
