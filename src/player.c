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

void player_respawn(player_t* player) {
  player->dirty_flag = 0;
  player->health = 100;
  player->shot_timer = 0;
  player->spec_timer = 0;
  player->swap_timer = 0;
  player->shooting = 0;
  player->phy.position.x = (rand() % 30) + 1.5;
  player->phy.position.y = (rand() % 30) + 1.5;
  player->phy.position.z = 0;
  player->phy.phi = 0;
  player->phy.velocity.x = 0;
  player->phy.velocity.y = 0;
  player->phy.velocity.z = 0;
  player->phy.friction = PLAYER_FRICTION;
  player->phy.bouncy = PLAYER_BOUNCY;
  player->dirty_flag = DIRTY_FLAG_HEALTH | DIRTY_FLAG_POSITION;
}

void player_shoot(player_t* player, sprite_bank_t* sprites) {
  int i;
  double x, y;
  sprite_t projectile;
  weapon_t weapon;
  double random_double;
  int prng = 125;

  if (player->shooting) {
    weapon = weapon_get(player->weapon);
    random_double = player->shooting / 256.0;
    for (i = 0; i < weapon.proj_cnt; i++) {
      prng = (prng * 3) % 257;
      x = (prng / 257.0 - 0.5);
      prng = (prng * 3) % 257;
      y = prng / 257.0 - 0.5;
      projectile = weapon.proj;
      projectile.owner = player->id;

      projectile.phy.position.x = player->phy.position.x;
      projectile.phy.position.y = player->phy.position.y;
      projectile.phy.position.z = player->phy.position.z;
      projectile.phy.phi = player->phy.phi;
      projectile.phy.friction = weapon.friction;
      projectile.phy.bouncy = weapon.bouncy;

      /* spray */
      projectile.phy.phi += (x + random_double) * weapon.spray / 200.0 - weapon.spray / 400.0;
      projectile.phy.velocity.x = cos(projectile.phy.phi) * projectile.vel;
      projectile.phy.velocity.y = sin(projectile.phy.phi) * projectile.vel;
      projectile.phy.velocity.z = (y + random_double) * weapon.spray / 10.0 - weapon.spray / 20.0;

      projectile.phy.position.x += cos(projectile.phy.phi) * (projectile.collision_radius + 0.1);
      projectile.phy.position.y += sin(projectile.phy.phi) * (projectile.collision_radius + 0.1);
      sprite_create(sprites, &projectile);
    }
    player->shot_timer = weapon.repeat_rate;
    player->shooting = 0;
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
    /* applly physics to local players */
    phy_update(&player->phy, map, 0, player_sprite->height, elapsed_time);
    player->dirty_flag |= DIRTY_FLAG_POSITION;
  } else {
    /* interpolate packets for remote players */
    player->net_interp += elapsed_time / (1000.0 / NET_FRAME_LIMIT);
    t = player->net_interp;
    player->phy.position.x =  t * player->net_this_pos.x + (1 - t) * player->net_last_pos.x;
    player->phy.position.y =  t * player->net_this_pos.y + (1 - t) * player->net_last_pos.y;
    player->phy.position.z =  t * player->net_this_pos.z + (1 - t) * player->net_last_pos.z;
    if (fabs(player->net_this_phi - player->net_last_phi) > TAU / 2) {
      if (player->net_this_phi > player->net_last_phi) {
        player->net_last_phi += TAU;
      } else {
        player->net_this_phi += TAU;
      }
    }
    player->phy.phi = fmod(player->net_last_phi + t * (player->net_this_phi - player->net_last_phi), TAU);
  }

  /* update position of sprite associated with player */
  player_sprite->phy = player->phy;

  /* detect player / sprite collisions */
  for (i = 0; i < sprites->size; i++) {
    sprite = &sprites->bank[i];
    if (!sprite->active || i == player->sprite) {
      continue;
    }
    distance = sqrt(SQUARED(sprite->phy.position.x - player->phy.position.x) + SQUARED(sprite->phy.position.y - player->phy.position.y));
    if (distance < sprite->collision_radius) {
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
  player->weapon = WEAPON_SHOTGUN;
  player->last_packet_time = 0;
  player->connected = 0;
  player->id = id;
  player->spec = id;
  /* sprite */
  player_sprite.active = 1;
  player_sprite.owner = id;
  player_sprite.height = 0.8;
  player_sprite.width = 0.5;
  player_sprite.color = GRAYSCALE(64);
  player_sprite.harm = 0;
  player_sprite.collision_radius = 0.5;
  player_sprite.collision_type = 0;
  player_sprite.exploding = 0;
  player_sprite.boom = 0;
  player_sprite.bounce = 0;
  player->sprite = sprite_create(sprites, &player_sprite);
}

int player_process_input(player_t* player, input_t* input, int elapsed_time) {
  double move_speed, rot_speed;

  move_speed = PLAYER_MOVE_SPEED * elapsed_time / 1000.0;
  rot_speed = PLAYER_ROT_SPEED * elapsed_time / 1000.0;

  input_update(input);
  if (input_is_pressed(input, INPUT_FORWARD)) {
    player->phy.velocity.x += cos(player->phy.phi) * move_speed;
    player->phy.velocity.y += sin(player->phy.phi) * move_speed;
  }
  if (input_is_pressed(input, INPUT_BACK)) {
    player->phy.velocity.x -= cos(player->phy.phi) * move_speed;
    player->phy.velocity.y -= sin(player->phy.phi) * move_speed;
  }
  if (input_is_pressed(input, INPUT_RIGHT)) {
    player->phy.velocity.x -= sin(player->phy.phi) * move_speed;
    player->phy.velocity.y += cos(player->phy.phi) * move_speed;
  }
  if (input_is_pressed(input, INPUT_LEFT)) {
    player->phy.velocity.x += sin(player->phy.phi) * move_speed;
    player->phy.velocity.y -= cos(player->phy.phi) * move_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_RIGHT)) {
    player->phy.phi += rot_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_LEFT)) {
    player->phy.phi -= rot_speed;
  }
  if (input_is_pressed(input, INPUT_CHANGE_GUN)) {
    if (player->swap_timer <= 0) {
      player->weapon = (player->weapon + 1) % MAX_WEAPON;
      player->dirty_flag |= DIRTY_FLAG_WEAPON;
      player->swap_timer = 250;
    }
  }
  if (input_is_pressed(input, INPUT_CHANGE_SPEC)) {
    if (player->spec_timer <= 0) {
      player->spec = (player->spec + 1) % MAX_PLAYERS;
      player->spec_timer = 250;
    }
  }
  if (input_is_pressed(input, INPUT_SHOOT)) {
    if (player->shot_timer <= 0) {
      player->shooting = (rand() % 255) + 1;
      player->dirty_flag |= DIRTY_FLAG_SHOOTING;
    }
  }
  if (player->shot_timer > 0) {
    player->shot_timer -= elapsed_time;
  }
  if (player->swap_timer > 0) {
    player->swap_timer -= elapsed_time;
  }
  if (player->spec_timer > 0) {
    player->spec_timer -= elapsed_time;
  }
  return input_is_pressed(input, INPUT_EXIT);
}

void player_cleanup(player_t* player) {
}

void player_harm(player_t* player, float damage) {
    player->health -= damage;
    player->dirty_flag |= DIRTY_FLAG_HEALTH;
    if (player->health <= 0) {
      player_respawn(player);
    }
}
