#include "player.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "physics.h"
#include "global.h"
#include "game.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define SQUARED(x) ((x)*(x))

void player_net_interpolate(player_t* player, int elapsed_time) {
  double t;

  player->remote.interp += elapsed_time / (1000.0 / NET_FRAME_LIMIT);
  t = player->remote.interp;
  player->object->physics.position.x =  t * player->remote.current_position.x + (1 - t) * player->remote.last_position.x;
  player->object->physics.position.y =  t * player->remote.current_position.y + (1 - t) * player->remote.last_position.y;
  player->object->physics.position.z =  t * player->remote.current_position.z + (1 - t) * player->remote.last_position.z;
  if (fabs(player->remote.current_rotation - player->remote.last_rotation) > TAU / 2) {
    if (player->remote.current_rotation > player->remote.last_rotation) {
      player->remote.last_rotation += TAU;
    } else {
      player->remote.current_rotation += TAU;
    }
  }
  player->object->physics.rotation = fmod(player->remote.last_rotation + t * (player->remote.current_rotation - player->remote.last_rotation), TAU);
}

void player_update(player_t* player, object_bank_t* objects, int elapsed_time) {

  int i = 0;
  double distance;
  object_t* object;

  /* interpolate positions of remote players */
  if (!player->local) {
    player_net_interpolate(player, elapsed_time);
  }

  /* detect player / object (including other players) collisions */
  for (i = 0; i < objects->size; i++) {
    object = &objects->bank[i];
    if (!object_is_valid(object) || object_is_exploding(object) || object_is_respawning(object) || object == player->object) {
      continue;
    }
    distance = SQUARED(object->physics.position.x - player->object->physics.position.x) + SQUARED(object->physics.position.y - player->object->physics.position.y);
    if (distance < object->collision_radius * object->collision_radius) {
      game_player_collide_with_object(player, object);
      object_collide_with_player(object);
    }
  }

}

void player_init(player_t* player, object_bank_t* objects, int local) {
  /* player */
  memset(player, 0, sizeof(player_t));
  player->local = local;
  game_player_init(player);
}

void player_cleanup(player_t* player) {
  game_player_cleanup(player);
}

int player_process_input(player_t* player, input_t* input, int elapsed_time) {
  double move_speed, rot_speed;

  move_speed = PLAYER_MOVE_SPEED * elapsed_time / 1000.0;
  rot_speed = PLAYER_ROT_SPEED * elapsed_time / 1000.0;

  input_update(input);

  if (input_is_pressed(input, INPUT_FORWARD)) {
    player->object->physics.velocity.x += cos(player->object->physics.rotation) * move_speed;
    player->object->physics.velocity.y += sin(player->object->physics.rotation) * move_speed;
  }

  if (input_is_pressed(input, INPUT_BACK)) {
    player->object->physics.velocity.x -= cos(player->object->physics.rotation) * move_speed;
    player->object->physics.velocity.y -= sin(player->object->physics.rotation) * move_speed;
  }

  if (input_is_pressed(input, INPUT_RIGHT)) {
    player->object->physics.velocity.x -= sin(player->object->physics.rotation) * move_speed;
    player->object->physics.velocity.y += cos(player->object->physics.rotation) * move_speed;
  }

  if (input_is_pressed(input, INPUT_LEFT)) {
    player->object->physics.velocity.x += sin(player->object->physics.rotation) * move_speed;
    player->object->physics.velocity.y -= cos(player->object->physics.rotation) * move_speed;
  }

  if (input_is_pressed(input, INPUT_ROTATE_RIGHT)) {
    player->object->physics.rotation += rot_speed;
  }

  if (input_is_pressed(input, INPUT_ROTATE_LEFT)) {
    player->object->physics.rotation -= rot_speed;
  }

  game_player_process_input(player, input, elapsed_time);

  return input_is_pressed(input, INPUT_EXIT);
}
