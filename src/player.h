#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "physics.h"
#include "global.h"

#define DIRTY_FLAG_POSITION (1 << 0)
#define DIRTY_FLAG_SHOOTING (1 << 1)
#define DIRTY_FLAG_HEALTH (1 << 2)
#define DIRTY_FLAG_WEAPON (1 << 3)

#define SHARE_SHOOTING (0)
#define SHARE_HEALTH (1)

/* network data for remote players */
typedef struct _player_remote_t {
  vec3_t   current_position;
  float    current_rotation;
  vec3_t   last_position;
  float    last_rotation;
  double   interp;
  int      packet_time;
} player_remote_t;

typedef struct _player_timers_t {
  int16_t  shot;
  int16_t  swap;
  int16_t  spec;
} player_timers_t;

typedef struct _player_t {
  object_t*       object;
  player_remote_t remote;
  player_timers_t timer;
  uint8_t         id;
  uint8_t         spec;
  uint8_t         weapon;
  uint8_t         share_flag;
  int16_t         share[8];
} player_t;

void player_init(player_t* player, object_bank_t* objects, int id);
void player_respawn(player_t* player);
void player_net_interpolate(player_t* player, int elapsed_time);
void player_update(player_t* player, object_bank_t* objects, int local, int elapsed_time);
int  player_process_input(player_t* player, input_t* input, int elapsed_time);
void player_cleanup(player_t* player);

void player_harm(player_t* player, float damage);

void player_share(player_t* player, int idx, uint16_t value);
void player_process_share(player_t* player, object_bank_t* objects);
void player_shoot(player_t* player, object_bank_t* objects);

#endif
