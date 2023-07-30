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
#define PLAYER_FRICTION (0.001)
#define PLAYER_BOUNCY (0.0)
#define PLAYER_MOVE_SPEED (20.0)
#define PLAYER_ROT_SPEED (3.14159)

void player_respawn(player_t* player, sprite_bank_t* sprites) {
  sprite_t* player_sprite;

  player->weapon = WEAPON_SHOTGUN;
  player->health = 100;
  player->shot_timer = 0;
  player->spec_timer = 0;
  player->swap_timer = 0;
  player->shooting = -1;
  player->phy.position.x = (rand() % 30) + 1.5;
  player->phy.position.y = (rand() % 30) + 1.5;
  player->phy.position.z = 0;
  player->phy.direction.x = 1;
  player->phy.direction.y = 0;
  player->phy.velocity.x = 0;
  player->phy.velocity.y = 0;
  player->phy.velocity.z = 0;
  player->phy.friction = PLAYER_FRICTION;
  player->phy.bouncy = PLAYER_BOUNCY;
  player->plane.x = 0;
  player->plane.y = 2.0 / 3.0;
  player_sprite = sprite_get(sprites, player->sprite);
  player_sprite->phy = player->phy;
}

void player_shoot(player_t* player, sprite_bank_t* sprites) {
  int i;
  double x, y;
  sprite_t projectile;
  weapon_t weapon;
  double phi;
  double random_double;
  int prng = 125;

  if (player->shooting > 0) {
    weapon = weapon_get(player->weapon);
    random_double = player->shooting;
    for (i = 0; i < weapon.proj_cnt; i++) {
      prng = (prng * 3) % 257;
      x = (prng / 257.0 - 0.5);
      prng = (prng * 3) % 257;
      y = prng / 257.0 - 0.5;
      projectile = weapon.proj;
      projectile.owner = player->id;

      /* shoot in the direction of the camera */
      projectile.phy.velocity.x = player->phy.direction.x;
      projectile.phy.velocity.y = player->phy.direction.y;
      projectile.phy.velocity.z = 0;
      projectile.phy.position.x = player->phy.position.x;
      projectile.phy.position.y = player->phy.position.y;
      projectile.phy.position.z = player->phy.position.z;
      projectile.phy.direction.x = player->phy.direction.x;
      projectile.phy.direction.y = player->phy.direction.y;
      projectile.phy.friction = weapon.friction;
      projectile.phy.bouncy = weapon.bouncy;

      /* spray */
      phi = atan2(projectile.phy.velocity.y, projectile.phy.velocity.x);
      phi += (x + random_double) * weapon.spray / 200.0 - weapon.spray / 400.0;
      projectile.phy.velocity.x = cos(phi) * projectile.vel;
      projectile.phy.velocity.y = sin(phi) * projectile.vel;
      projectile.phy.position.x += projectile.phy.direction.x * (projectile.harm_radius + 0.1);
      projectile.phy.position.y += projectile.phy.direction.y * (projectile.harm_radius + 0.1);
      projectile.phy.velocity.z += (y + random_double) * weapon.spray / 10.0 - weapon.spray / 20.0;
      sprite_create(sprites, &projectile);
    }
    player->shot_timer = weapon.repeat_rate;
    player->shooting = -1;
  }
}

void player_update(player_t* player, sprite_bank_t* sprites, map_t* map, int elapsed_time) {
  int hurt_me, attacker;
  sprite_t* player_sprite;

  player_sprite = sprite_get(sprites, player->sprite);
  phy_update(&player->phy, map, -1, player_sprite->height, elapsed_time);
  /* update sprite position */
  player_sprite->phy = player->phy;
  /* detect damage */
  sprite_sort_by_dist(sprites, player->phy.position, &hurt_me, &attacker);
  if (attacker >= 0) {
    player->health -= hurt_me;
    if (player->health <= 0) {
      player_respawn(player, sprites);
    }
  }
}

void player_init(player_t* player, sprite_bank_t* sprites, int id) {
  sprite_t player_sprite;

  player_sprite.active = 1;
  player_sprite.owner = 0;
  /* player_sprite.phy */
  player_sprite.phy.position.z = 0.0;
  player_sprite.vel = 0;
  /* player_sprite.friction */
  player_sprite.height = 0.8;
  player_sprite.width = 0.5;
  player_sprite.color = GRAYSCALE(64);
  player_sprite.harm = 0;
  player_sprite.harm_radius = 0;
  player_sprite.boom = 0;
  player_sprite.bounce = 0;

  player->sprite = sprite_create(sprites, &player_sprite);
  player->id = id;
  player->spec = id;
  player->connected = 0;
}

int player_process_input(player_t* player, input_t* input, int elapsed_time) {
  double move_speed, rot_speed;

  move_speed = PLAYER_MOVE_SPEED * elapsed_time / 1000.0;
  rot_speed = PLAYER_ROT_SPEED * elapsed_time / 1000.0;

  input_update(input);
  if (input_is_pressed(input, INPUT_FORWARD)) {
    player->phy.velocity.x += player->phy.direction.x * move_speed;
    player->phy.velocity.y += player->phy.direction.y * move_speed;
  }
  if (input_is_pressed(input, INPUT_BACK)) {
    player->phy.velocity.x -= player->phy.direction.x * move_speed;
    player->phy.velocity.y -= player->phy.direction.y * move_speed;
  }
  if (input_is_pressed(input, INPUT_RIGHT)) {
    player->phy.velocity.x -= player->phy.direction.y * move_speed;
    player->phy.velocity.y += player->phy.direction.x * move_speed;
  }
  if (input_is_pressed(input, INPUT_LEFT)) {
    player->phy.velocity.x += player->phy.direction.y * move_speed;
    player->phy.velocity.y -= player->phy.direction.x * move_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_RIGHT)) {
    phy_rotate(&player->phy.direction, rot_speed);
    phy_rotate(&player->plane, rot_speed);
  }
  if (input_is_pressed(input, INPUT_ROTATE_LEFT)) {
    phy_rotate(&player->phy.direction, -rot_speed);
    phy_rotate(&player->plane, -rot_speed);
  }
  if (input_is_pressed(input, INPUT_CHANGE_GUN)) {
    if (player->swap_timer <= 0) {
      player->weapon = (player->weapon + 1) % MAX_WEAPON;
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
      player->shooting = rand() / (double) RAND_MAX;
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

