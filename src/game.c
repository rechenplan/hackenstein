#include "game.h"
#include "player.h"
#include "object.h"
#include "weapon.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static void game_spawn_pickup(vec3_t pos, object_bank_t* objects) {
  object_t* pickup;
  pickup = object_create(objects);
  pickup->physics.position = pos;
  pickup->next_respawn = 60;
  pickup->respawn_time = -1;
  pickup->physics.friction = 0.999;
  pickup->physics.bouncy = 1.0;
  pickup->height = 0.25;
  pickup->width = 0.25;
  pickup->color = 0xffffff00;
  pickup->harm = -10;
  pickup->collision_radius = 2.0;
  pickup->boom = 0;
  pickup->type = OBJECT_TYPE_PICKUP;
}

static void game_share(player_t* player, int idx, uint16_t value) {
  player->share_flag |= (1 << idx);
  player->share[idx] = value;
}

static void game_harm(player_t* player, object_t* object) {
    game_share(player, SHARE_HEALTH, player->share[SHARE_HEALTH] - object->harm);
}

static void game_shoot_check(player_t* player, object_bank_t* objects) {
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
    for (i = 0; i < weapon.projectile_count; i++) {
      prng = (prng * 3) % 257;
      x = (prng / 257.0 - 0.5);
      prng = (prng * 3) % 257;
      y = prng / 257.0 - 0.5;

      projectile = object_create(objects);

      memcpy(projectile, &weapon.projectile, sizeof(object_t));
      projectile->owner = player;

      projectile->physics.position.x = player->object->physics.position.x;
      projectile->physics.position.y = player->object->physics.position.y;
      projectile->physics.position.z = player->object->physics.position.z;
      projectile->physics.rotation = player->object->physics.rotation;

      /* spray */
      projectile->physics.rotation += (x + random_double) * weapon.spray / 200.0 - weapon.spray / 400.0;
      projectile->physics.velocity.x = cos(projectile->physics.rotation) * weapon.projectile_speed;
      projectile->physics.velocity.y = sin(projectile->physics.rotation) * weapon.projectile_speed;
      projectile->physics.velocity.z = (y + random_double) * weapon.spray / 10.0 - weapon.spray / 20.0;

      projectile->physics.position.x += cos(projectile->physics.rotation) * (projectile->collision_radius + 0.1);
      projectile->physics.position.y += sin(projectile->physics.rotation) * (projectile->collision_radius + 0.1);
    }
    player->timer.shot = 5;
    player->share[SHARE_SHOOTING] = 0;
  }
}

static void game_respawn(player_t* player) {
  player->share_flag = 0;
  player->timer.shot = 0;
  player->timer.swap = 0;
  player->object->physics.position.x = (rand() % 30) + 1.5;
  player->object->physics.position.y = (rand() % 30) + 1.5;
  player->object->physics.position.z = 0;
  player->object->physics.rotation = 0;
  player->object->physics.velocity.x = 0;
  player->object->physics.velocity.y = 0;
  player->object->physics.velocity.z = 0;
  player->object->physics.friction = PLAYER_FRICTION;
  player->object->physics.bouncy = PLAYER_BOUNCY;
  game_share(player, SHARE_HEALTH, 100);
}

void game_player_init(player_t* player, object_bank_t* objects) {
  player->weapon = WEAPON_SHOTGUN;
  player->object = object_create(objects);
  player->object->active = 1;
  player->object->owner = player;
  player->object->height = 0.8;
  player->object->width = 0.5;
  player->object->color = GRAYSCALE(64);
  player->object->collision_radius = 0.5;
  player->object->type = OBJECT_TYPE_PLAYER;
  player->object->bounces_left = 0;
  game_respawn(player);
}

void game_map_load(map_t* map) {
  int i, j;
  for (j = 1; j < map->height; j++) {
    for (i = 1; i < map->width; i++) {
      map_set_cell(map, i, j, 0);
    }
  }
  map_set_cell(map, 7, 7, 1);
}

void game_player_cleanup(player_t* player) {
}

void game_player_collide_with_object(player_t* player, object_t* object) {
  if (player->local) {
    game_harm(player, object);
  }
}

void game_player_update(player_t* player, object_bank_t* objects) {
  game_shoot_check(player, objects);
  if (player->share[SHARE_HEALTH] <= 0) {
    game_spawn_pickup(player->object->physics.position, objects);
    game_respawn(player);
  }

  if (player->timer.shot > 0) {
    player->timer.shot--;
  }

  if (player->timer.swap > 0) {
    player->timer.swap--;
  }
}


void game_key_up(player_t* player, int key) {
  return;
}

void game_key_down(player_t* player, int key) {
  if (key == 'o') {
    if (player->timer.swap <= 0) {
      player->weapon = (player->weapon + 1) % MAX_WEAPON;
      player->timer.swap = 250;
    }
  }

  if (key == ' ') {
    if (player->timer.shot <= 0) {
      game_share(player, SHARE_SHOOTING, (player->weapon << 8) | ((rand() % 255) + 1));
    }
  }

  return;
}
