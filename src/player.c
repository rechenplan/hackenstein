#include "player.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "phy.h"
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

void player_process_share(player_t* player, sprite_bank_t* sprites) {
  player_shoot(player, sprites);
  if (player->share[SHARE_HEALTH] <= 0) {
    player_respawn(player);
  }
}

#define SQUARED(x) ((x)*(x))

void player_update(player_t* player, sprite_bank_t* sprites, map_t* map, int elapsed_time, int local) {

  sprite_t* player_sprite;
  sprite_t* sprite;
  int i = 0;
  double distance, t;

  player_sprite = sprite_get(sprites, player->sprite);
  if (local) {

    /* apply physics to local players */
    phy_update(&player->physical, map, 0, player_sprite->height, elapsed_time);

  } else {

    /* interpolate packets for remote players */
    player->remote.interp += elapsed_time / (1000.0 / NET_FRAME_LIMIT);
    t = player->remote.interp;
    player->physical.position.x =  t * player->remote.current_position.x + (1 - t) * player->remote.last_position.x;
    player->physical.position.y =  t * player->remote.current_position.y + (1 - t) * player->remote.last_position.y;
    player->physical.position.z =  t * player->remote.current_position.z + (1 - t) * player->remote.last_position.z;
    if (fabs(player->remote.current_rotation - player->remote.last_rotation) > TAU / 2) {
      if (player->remote.current_rotation > player->remote.last_rotation) {
        player->remote.last_rotation += TAU;
      } else {
        player->remote.current_rotation += TAU;
      }
    }
    player->physical.rotation = fmod(player->remote.last_rotation + t * (player->remote.current_rotation - player->remote.last_rotation), TAU);

  }

  /* update position of sprite associated with player */
  player_sprite->physical = player->physical;

  /* detect player / sprite collisions */
  for (i = 0; i < sprites->size; i++) {
    sprite = sprite_get(sprites, i);
    if (!sprite->active || i == player->sprite) {
      continue;
    }
    distance = SQUARED(sprite->physical.position.x - player->physical.position.x) + SQUARED(sprite->physical.position.y - player->physical.position.y);
    if (distance < sprite->collision_radius * sprite->collision_radius) {
      if (local) {
        player_harm(player, sprite->harm);
      }
      sprite_collide(sprite);
    }
  }
}

void player_init(player_t* player, sprite_bank_t* sprites, int id) {
  sprite_t player_sprite;
  /* player */

  memset(player, 0, sizeof(player_t));
  player->id = id;
  player->spec = id;

  /* SCRIPTME */
  player->weapon = WEAPON_SHOTGUN;

  player_sprite.active = 1;
  player_sprite.owner = id;
  player_sprite.height = 0.8;
  player_sprite.width = 0.5;
  player_sprite.color = GRAYSCALE(64);
  player_sprite.harm = 0;
  player_sprite.collision_radius = 0.5;
  player_sprite.collision_type = COLLISION_NONE;
  player_sprite.exploding = 0;
  player_sprite.boom = 0;
  player_sprite.bounces_left = 0;
  player->sprite = sprite_create(sprites, &player_sprite);
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
    player->physical.velocity.x += cos(player->physical.rotation) * move_speed;
    player->physical.velocity.y += sin(player->physical.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_BACK)) {
    player->physical.velocity.x -= cos(player->physical.rotation) * move_speed;
    player->physical.velocity.y -= sin(player->physical.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_RIGHT)) {
    player->physical.velocity.x -= sin(player->physical.rotation) * move_speed;
    player->physical.velocity.y += cos(player->physical.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_LEFT)) {
    player->physical.velocity.x += sin(player->physical.rotation) * move_speed;
    player->physical.velocity.y -= cos(player->physical.rotation) * move_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_RIGHT)) {
    player->physical.rotation += rot_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_LEFT)) {
    player->physical.rotation -= rot_speed;
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
void player_shoot(player_t* player, sprite_bank_t* sprites) {
  int i;
  double x, y;
  sprite_t projectile;
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
      projectile = weapon.proj;
      projectile.owner = player->id;

      projectile.physical.position.x = player->physical.position.x;
      projectile.physical.position.y = player->physical.position.y;
      projectile.physical.position.z = player->physical.position.z;
      projectile.physical.rotation = player->physical.rotation;
      projectile.physical.friction = weapon.friction;
      projectile.physical.bouncy = weapon.bouncy;

      /* spray */
      projectile.physical.rotation += (x + random_double) * weapon.spray / 200.0 - weapon.spray / 400.0;
      projectile.physical.velocity.x = cos(projectile.physical.rotation) * weapon.proj_speed;
      projectile.physical.velocity.y = sin(projectile.physical.rotation) * weapon.proj_speed;
      projectile.physical.velocity.z = (y + random_double) * weapon.spray / 10.0 - weapon.spray / 20.0;

      projectile.physical.position.x += cos(projectile.physical.rotation) * (projectile.collision_radius + 0.1);
      projectile.physical.position.y += sin(projectile.physical.rotation) * (projectile.collision_radius + 0.1);
      sprite_create(sprites, &projectile);
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
  player->physical.position.x = (rand() % 30) + 1.5;
  player->physical.position.y = (rand() % 30) + 1.5;
  player->physical.position.z = 0;
  player->physical.rotation = 0;
  player->physical.velocity.x = 0;
  player->physical.velocity.y = 0;
  player->physical.velocity.z = 0;
  player->physical.friction = PLAYER_FRICTION;
  player->physical.bouncy = PLAYER_BOUNCY;
  player_share(player, SHARE_HEALTH, 100);
}

