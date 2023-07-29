#include "player.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "phy.h"
#include "weapon.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

void player_respawn(player_t* player) {
  player->weapon = WEAPON_SMG;
  player->health = 100;
  player->shot_timer = 0;
  player->me.pos_x = 8.5;
  player->me.pos_y = 8.5;
  player->me.dir_x = 1;
  player->me.dir_y = 0;
  player->me.vel_x = 0;
  player->me.vel_y = 0;
  player->me.vel_z = 0;
  player->camera_plane.dir_x = 0;
  player->camera_plane.dir_y = 2.0 / 3.0;
}

void player_shot_update(player_t* player, sprite_bank_t* sprites) {
  int i;
  double phi;
  sprite_t projectile;
  weapon_t weapon;
  double random_double;

  weapon = weapon_get(player->weapon);
  if (player->shooting > 0) {
    random_double = player->shooting;
    for (i = 0; i < weapon.proj_cnt; i++) {
      projectile = weapon.proj;
      projectile.phy = player->me;
      /* shoot in the direction of the camera */
      projectile.phy.vel_x = projectile.phy.dir_x * weapon.proj.vel;
      projectile.phy.vel_y = projectile.phy.dir_y * weapon.proj.vel;
      projectile.phy.vel_z = 0;
      projectile.phy.pos_z = 0.35;
      /* spray */
      phi = atan2(projectile.phy.vel_y, projectile.phy.vel_x);
      phi += random_double * weapon.spray / 100.0 - weapon.spray / 200.0;
      projectile.phy.vel_x = cos(phi);
      projectile.phy.vel_y = sin(phi);
      projectile.phy.pos_x += projectile.phy.vel_x * (projectile.harm_radius + 0.1);
      projectile.phy.pos_y += projectile.phy.vel_y * (projectile.harm_radius + 0.1);
      projectile.phy.vel_z += (random_double / 1000.0) * weapon.spray - weapon.spray / 2000.0;
      sprite_create(sprites, &projectile);
    }
    player->shot_timer = weapon.repeat_rate;
  }
}

void player_init(player_t* player) {
}

int player_local_update(player_t* player, input_t* input, map_t* map, sprite_bank_t* sprites) {
  double move_speed = 0.01, rot_speed = 0.05, friction = 0.9;

  input_update(input);
  player->me.vel_x *= friction;
  player->me.vel_y *= friction;
  if (input_is_pressed(input, INPUT_FORWARD)) {
    player->me.vel_x += player->me.dir_x * move_speed;
    player->me.vel_y += player->me.dir_y * move_speed;
  }
  if (input_is_pressed(input, INPUT_BACK)) {
    player->me.vel_x -= player->me.dir_x * move_speed;
    player->me.vel_y -= player->me.dir_y * move_speed;
  }
  if (input_is_pressed(input, INPUT_RIGHT)) {
    player->me.vel_x -= player->me.dir_y * move_speed;
    player->me.vel_y += player->me.dir_x * move_speed;
  }
  if (input_is_pressed(input, INPUT_LEFT)) {
    player->me.vel_x += player->me.dir_y * move_speed;
    player->me.vel_y -= player->me.dir_x * move_speed;
  }
  if (input_is_pressed(input, INPUT_ROTATE_RIGHT)) {
    phy_rotate(&player->me, rot_speed);
    phy_rotate(&player->camera_plane, rot_speed);
  }
  if (input_is_pressed(input, INPUT_ROTATE_LEFT)) {
    phy_rotate(&player->me, -rot_speed);
    phy_rotate(&player->camera_plane, -rot_speed);
  }
  player->shooting = -1.0;
  if (input_is_pressed(input, INPUT_SHOOT)) {
    if (!player->shot_timer) {
      player->shooting = rand() / (double) RAND_MAX;
    }
  }
  if (player->shot_timer > 0) {
    player->shot_timer--;
  }
  phy_rel_move(&player->me, map, 0, 1, 0);
  return input_is_pressed(input, INPUT_EXIT);
}

void player_cleanup(player_t* player) {
}

