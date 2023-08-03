#ifndef PLAYER_STRUCT_H
#define PLAYER_STRUCT_H

#include "physics.h"
#include "object.h"

/* network data for remote players */
typedef struct _player_remote_t {
  vec3_t   current_position;
  float    current_rotation;
  vec3_t   last_position;
  float    last_rotation;
  double   interp;
  int      packet_time;
} player_remote_t;

typedef struct _player_t {
  object_t*       object;
  player_remote_t remote;
  uint8_t         local;
  int             connected;
} player_t;

#endif
