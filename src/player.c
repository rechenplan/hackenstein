#include "player.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "physics.h"
#include "weapon.h"
#include "global.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* per second */
#define PLAYER_FRICTION (1.0 / 100000.0)
#define PLAYER_BOUNCY (0.0)
#define PLAYER_MOVE_SPEED (50.0)
#define PLAYER_ROT_SPEED (TAU / 4)

void player_share(player_t* player, int idx, uint16_t value) {
  player->share_flag |= (1 << idx);
  player->share[idx] = value;
}

void player_process_share(player_t* player, object_bank_t* objects) {
  player_shoot(player, objects);
  if (player->share[SHARE_HEALTH] <= 0) {
    player_respawn(player);
  }
}

#define SQUARED(x) ((x)*(x))

void player_update(player_t* player, object_bank_t* objects, map_t* map, int elapsed_time, int local) {

  int i = 0;
  double distance, t;
  object_t* object;

  if (local) {

    /* apply physics to local players */
    physics_update(&player->physics, map, 0, player->object->height, elapsed_time);

  } else {

    /* interpolate packets for remote players */
    player->remote.interp += elapsed_time / (1000.0 / NET_FRAME_LIMIT);
    t = player->remote.interp;
    player->physics.position.x =  t * player->remote.current_position.x + (1 - t) * player->remote.last_position.x;
    player->physics.position.y =  t * player->remote.current_position.y + (1 - t) * player->remote.last_position.y;
    player->physics.position.z =  t * player->remote.current_position.z + (1 - t) * player->remote.last_position.z;
    if (fabs(player->remote.current_rotation - player->remote.last_rotation) > TAU / 2) {
      if (player->remote.current_rotation > player->remote.last_rotation) {
        player->remote.last_rotation += TAU;
      } else {
        player->remote.current_rotation += TAU;
      }
    }
    player->physics.rotation = fmod(player->remote.last_rotation + t * (player->remote.current_rotation - player->remote.last_rotation), TAU);

  }

  /* update position of object associated with player */
  player->object->physics = player->physics;

  /* detect player / object collisions */
  for (i = 0; i < objects->size; i++) {
    object = &objects->bank[i];
    if (!object->active || object == player->object) {
      continue;
    }
    distance = SQUARED(object->physics.position.x - player->physics.position.x) + SQUARED(object->physics.position.y - player->physics.position.y);
    if (distance < object->collision_radius * object->collision_radius) {
      if (local) {
        player_harm(player, object->harm);
      }
      object_collide(object);
    }
  }
}

void player_init(player_t* player, object_bank_t* objects, int id) {
  /* player */
  memset(player, 0, sizeof(player_t));
  player->id = id;
  player->spec = id;

  /* SCRIPTME */
  player->weapon = WEAPON_SHOTGUN;

  player->object = object_create(objects);

  player->object->active = 1;
  player->object->owner = id;
  player->object->height = 0.8;
  player->object->width = 0.5;
  player->object->color = GRAYSCALE(64);
  player->object->harm = 0;
  player->object->collision_radius = 0.5;
  player->object->collision_type = COLLISION_NONE;
  player->object->exploding = 0;
  player->object->boom = 0;
  player->object->bounces_left = 0;
  /* ^^^ */
}

void player_cleanup(player_t* player) {
}

int player_process_input(player_t* player, input_t* input, int elapsed_time) {
  double move_speed, rot_speed;

  move_speed = PLAYER_MOVE_SPEED * elapsed_time / 1000.0;
  rot_speed = PLAYER_ROT_SPEED * elapsed_time / 1000.0;

  input_update(input);
  if (input_is_pressed(input, INPUT_FORWARD)) {
    player->physics.velocity.x += cos(player->physics.rotation) * move_speed;
    player->physics.velocity.y += sin(player->physics.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_BACK)) {
    player->physics.velocity.x -= cos(player->physics.rotation) * move_speed;
    player->physics.velocity.y -= sin(player->physics.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_RIGHT)) {
    player->physics.velocity.x -= sin(player->physics.rotation) * move_speed;
    player->physics.velocity.y += cos(player->physics.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_LEFT)) {
    player->physics.velocity.x += sin(player->physics.rotation) * move_speed;
    player->physics.velocity.y -= cos(player->physics.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_RIGHT)) {
    player->physics.rotation += rot_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_LEFT)) {
    player->physics.rotation -= rot_speed;
  }

  /* SCRIPTME */
  if (input_is_pressed(input, INPUT_CHANGE_GUN)) {
    if (player->timer.swap <= 0) {
      player->weapon = (player->weapon + 1) % MAX_WEAPON;
      player->timer.swap = 250;
    }
  }
  if (input_is_pressed(input, INPUT_CHANGE_SPEC)) {
    if (player->timer.spec <= 0) {
      player->spec = (player->spec + 1) % MAX_PLAYERS;
      player->timer.spec = 250;
    }
  }
  if (input_is_pressed(input, INPUT_SHOOT)) {
    if (player->timer.shot <= 0) {
      player_share(player, SHARE_SHOOTING, (player->weapon << 8) | ((rand() % 255) + 1));
    }
  }
  if (player->timer.shot > 0) {
    player->timer.shot -= elapsed_time;
  }
  if (player->timer.swap > 0) {
    player->timer.swap -= elapsed_time;
  }
  if (player->timer.spec > 0) {
    player->timer.spec -= elapsed_time;
  }
  /* ^^^ */

  return input_is_pressed(input, INPUT_EXIT);
}

/* SCRIPTME */
void player_harm(player_t* player, float damage) {
    player_share(player, SHARE_HEALTH, player->share[SHARE_HEALTH] - damage);
}

/* SCRIPTME */
void player_shoot(player_t* player, object_bank_t* objects) {
  int i;
  double x, y;
  object_t* projectile;
  weapon_t weapon;
  double random_double;
  int prng = 125;
  int weapon_id;

  if (player->share[SHARE_SHOOTING]) {
    weapon_id = player->share[SHARE_SHOOTING] >> 8;
    weapon = weapon_get(weapon_id);
    random_double = (player->share[SHARE_SHOOTING] & 255) / 256.0;
    for (i = 0; i < weapon.proj_cnt; i++) {
      prng = (prng * 3) % 257;
      x = (prng / 257.0 - 0.5);
      prng = (prng * 3) % 257;
      y = prng / 257.0 - 0.5;

      projectile = object_create(objects);

      memcpy(projectile, &weapon.proj, sizeof(object_t));
      projectile->owner = player->id;

      projectile->physics.position.x = player->physics.position.x;
      projectile->physics.position.y = player->physics.position.y;
      projectile->physics.position.z = player->physics.position.z;
      projectile->physics.rotation = player->physics.rotation;
      projectile->physics.friction = weapon.friction;
      projectile->physics.bouncy = weapon.bouncy;

      /* spray */
      projectile->physics.rotation += (x + random_double) * weapon.spray / 200.0 - weapon.spray / 400.0;
      projectile->physics.velocity.x = cos(projectile->physics.rotation) * weapon.proj_speed;
      projectile->physics.velocity.y = sin(projectile->physics.rotation) * weapon.proj_speed;
      projectile->physics.velocity.z = (y + random_double) * weapon.spray / 10.0 - weapon.spray / 20.0;

      projectile->physics.position.x += cos(projectile->physics.rotation) * (projectile->collision_radius + 0.1);
      projectile->physics.position.y += sin(projectile->physics.rotation) * (projectile->collision_radius + 0.1);
    }
    player->timer.shot = weapon.repeat_rate;
    player->share[SHARE_SHOOTING] = 0;
  }
}

/* SCRIPTME */
void player_respawn(player_t* player) {
  player->share_flag = 0;
  player->timer.shot = 0;
  player->timer.spec = 0;
  player->timer.swap = 0;
  player->physics.position.x = (rand() % 30) + 1.5;
  player->physics.position.y = (rand() % 30) + 1.5;
  player->physics.position.z = 0;
  player->physics.rotation = 0;
  player->physics.velocity.x = 0;
  player->physics.velocity.y = 0;
  player->physics.velocity.z = 0;
  player->physics.friction = PLAYER_FRICTION;
  player->physics.bouncy = PLAYER_BOUNCY;
  player_share(player, SHARE_HEALTH, 100);
}

