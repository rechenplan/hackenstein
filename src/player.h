#ifndef PLAYER_H
#define PLAYER_H

#include "sprite.h"
#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "phy.h"
#include "global.h"

#define DIRTY_FLAG_POSITION (1 << 0)
#define DIRTY_FLAG_SHOOTING (1 << 1)
#define DIRTY_FLAG_HEALTH (1 << 2)
#define DIRTY_FLAG_WEAPON (1 << 3)

/* network data for remote players */
typedef struct _player_remote_t {
  vec3_t   current_pos;
  float    current_phi;
  vec3_t   last_pos;
  float    last_phi;
  double   interp;
  int      packet_time;
} player_remote_t;

typedef struct _player_t {
  phy_t           phy;
  player_remote_t remote;
  uint8_t         id;

  uint8_t  dirty_flag;
  uint8_t  weapon;
  float    health;
  uint8_t  shooting;

  int16_t  shot_timer;
  int16_t  swap_timer;
  int16_t  spec_timer;
  uint16_t sprite;
  uint8_t  spec;
} player_t;

void player_init(player_t* player, sprite_bank_t* sprites, int id);
void player_respawn(player_t* player);
void player_update(player_t* player, sprite_bank_t* sprites, map_t* map, int elapsed_time, int local);
void player_shoot(player_t* player, sprite_bank_t* sprites);
int  player_process_input(player_t* player, input_t* input, int elapsed_time);
void player_cleanup(player_t* player);
void player_sprite_collision(player_t* player, sprite_t* sprite, int elapsed_time);
void player_harm(player_t* player, float damage);

#endif
