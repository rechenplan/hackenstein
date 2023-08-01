#include "player.h"
#include "object.h"
#include "weapon.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static void mod_spawn_pickup(vec3_t pos, object_bank_t* objects) {
  object_t* pickup;
  pickup = object_create(objects);
  pickup->physics.position = pos;
  pickup->physics.velocity = 
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

static void mod_share(player_t* player, int idx, uint16_t value) {
  player->share_flag |= (1 << idx);
  player->share[idx] = value;
}

static void mod_harm(player_t* player, object_t* object) {
    mod_share(player, SHARE_HEALTH, player->share[SHARE_HEALTH] - object->harm);
}

static void mod_shoot_check(player_t* player, object_bank_t* objects) {
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
    player->timer.shot = weapon.repeat_rate;
    player->share[SHARE_SHOOTING] = 0;
  }
}

static void mod_respawn(player_t* player) {
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
  mod_share(player, SHARE_HEALTH, 100);
}

void mod_player_init(player_t* player, object_bank_t* objects) {
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
  mod_respawn(player);
}

void mod_player_cleanup(player_t* player) {
}

void mod_player_collide_with_object(player_t* player, object_t* object) {
  if (player->local) {
    mod_harm(player, object);
  }
}

void mod_player_update(player_t* player, object_bank_t* objects) {
  mod_shoot_check(player, objects);
  if (player->share[SHARE_HEALTH] <= 0) {
    mod_spawn_pickup(player->object->physics.position, objects);
    mod_respawn(player);
  }
}

/* TODO: change this to trigger on key down only. right now it triggers every physics frame. */
void mod_player_process_input(player_t* player, input_t* input, int elapsed_time) {
  if (input_is_pressed(input, INPUT_CHANGE_GUN)) {
    if (player->timer.swap <= 0) {
      player->weapon = (player->weapon + 1) % MAX_WEAPON;
      player->timer.swap = 250;
    }
  }

  if (input_is_pressed(input, INPUT_SHOOT)) {
    if (player->timer.shot <= 0) {
      mod_share(player, SHARE_SHOOTING, (player->weapon << 8) | ((rand() % 255) + 1));
    }
  }

  if (player->timer.shot > 0) {
    player->timer.shot -= elapsed_time;
  }
  if (player->timer.swap > 0) {
    player->timer.swap -= elapsed_time;
  }
  return;
}
