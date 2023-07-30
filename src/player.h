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
  phy_t    phy; /* 28 bytes need to be sent (position & direction) */

  /* these 4 bytes need to be sent */
  uint8_t  id;
  uint8_t  weapon;
  int8_t   health;
  uint8_t  shooting;


  vec2_t   plane; /* don't need to end this (can be reconstructed from dir) */
  int16_t  shot_timer;
  int16_t  swap_timer;
  int16_t  spec_timer;
  uint16_t sprite;
  uint8_t  spec;
  uint8_t  connected;
} player_t;

void player_init(player_t* player, sprite_bank_t* sprites, int id);
void player_respawn(player_t* player, sprite_bank_t* sprites);
void player_update(player_t* player, sprite_bank_t* sprites, map_t* map, int elapsed_time);
void player_shoot(player_t* player, sprite_bank_t* sprites);
int  player_process_input(player_t* player, input_t* input, int elapsed_time);
void player_cleanup(player_t* player);

#endif
