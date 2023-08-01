#ifndef PLAYER_H
#define PLAYER_H

#include "object.h"
#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "physics.h"
#include "global.h"

/* per second */
#define PLAYER_FRICTION (1.0 / 100000.0)
#define PLAYER_BOUNCY (0.0)
#define PLAYER_MOVE_SPEED (50.0)
#define PLAYER_ROT_SPEED (TAU / 4)

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
  uint8_t         local;

  /* mod stuff */
  player_timers_t timer;
  uint8_t         weapon;
  uint8_t         share_flag;
  int16_t         share[8];
} player_t;

void player_init(player_t* player, object_bank_t* objects, int id);
void player_update(player_t* player, object_bank_t* objects, int elapsed_time);
int  player_process_input(player_t* player, input_t* input, int elapsed_time);
void player_net_interpolate(player_t* player, int elapsed_time);
void player_cleanup(player_t* player);

#endif
