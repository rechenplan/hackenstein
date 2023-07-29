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
  player->id = 2;
  player->weapon = WEAPON_SHOTGUN;
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

void player_update(player_t* player, sprite_bank_t* sprites, map_t* map) {
  double friction = 0.9;
  int i;
  double x, y;
  double phi;
  sprite_t projectile;
  weapon_t weapon;
  double random_double;
  int prng = 125;
  weapon = weapon_get(player->weapon);
  if (player->shooting > 0) {
    random_double = player->shooting;

    for (i = 0; i < weapon.proj_cnt; i++) {
      prng = (prng * 3) % 257;
      x = (prng / 257.0 - 0.5);
      prng = (prng * 3) % 257;
      y = prng / 257.0 - 0.5;
      projectile = weapon.proj;
      projectile.owner = player->id;
      projectile.phy = player->me;
      /* shoot in the direction of the camera */
      projectile.phy.vel_x = projectile.phy.dir_x * weapon.proj.vel;
      projectile.phy.vel_y = projectile.phy.dir_y * weapon.proj.vel;
      projectile.phy.vel_z = 0;
      projectile.phy.pos_z = 0.35;
      /* spray */
      phi = atan2(projectile.phy.vel_y, projectile.phy.vel_x);
      phi += (x + random_double) * weapon.spray / 200.0 - weapon.spray / 400.0;
      projectile.phy.vel_x = cos(phi);
      projectile.phy.vel_y = sin(phi);
      projectile.phy.pos_x += projectile.phy.vel_x * (projectile.harm_radius + 0.1);
      projectile.phy.pos_y += projectile.phy.vel_y * (projectile.harm_radius + 0.1);
      projectile.phy.vel_z += (y + random_double) * weapon.spray / 2000.0 - weapon.spray / 4000.0;
      sprite_create(sprites, &projectile);
    }
    player->shot_timer = weapon.repeat_rate;
  }
  player->me.vel_x *= friction;
  player->me.vel_y *= friction;
  phy_rel_move(&player->me, map, 0, 1, 0);
}

void player_init(player_t* player) {
}

int player_process_input(player_t* player, input_t* input) {
  double move_speed = 0.01, rot_speed = 0.05;

  input_update(input);
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
  return input_is_pressed(input, INPUT_EXIT);
}

void player_cleanup(player_t* player) {
}

