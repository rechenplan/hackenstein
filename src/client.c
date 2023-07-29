#include "client.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "phy.h"
#include "weapon.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static void client_respawn(client_t* client) {
  client->shot_timer = 0;
  client->camera.pos_x = 8.5;
  client->camera.pos_y = 8.5;
  client->camera.dir_x = 1;
  client->camera.dir_y = 0;
  client->camera.vel_x = 0;
  client->camera.vel_y = 0;
  client->camera.vel_z = 0;
  client->camera_plane.dir_x = 0;
  client->camera_plane.dir_y = 2.0 / 3.0;
}

static void client_shoot(client_t* client) {
  int i;
  double phi;
  sprite_t projectile;
  double random_double;
  weapon_t weapon;

  weapon = weapon_get(WEAPON_SMG);


  /* debug */
  /*
    projectile.phy.pos_x = 8;
    projectile.phy.pos_y = 8;
    projectile.phy.dir_x = 1;
    projectile.phy.dir_y = 0;
  */
  if (!client->shot_timer) {
    for (i = 0; i < weapon.proj_cnt; i++) {
      projectile = weapon.proj;
      projectile.phy = client->camera;
      /* shoot in the direction of the camera */
      projectile.phy.vel_x = projectile.phy.dir_x * weapon.proj.vel;
      projectile.phy.vel_y = projectile.phy.dir_y * weapon.proj.vel;
      projectile.phy.vel_z = 0;
      projectile.phy.pos_z = 0.35;
      /* spray */
      random_double = (rand() / (double) RAND_MAX) / 100.0;
      phi = atan2(projectile.phy.vel_y, projectile.phy.vel_x);
      phi += random_double * weapon.spray - weapon.spray / 200.0;
      projectile.phy.vel_x = cos(phi);
      projectile.phy.vel_y = sin(phi);
      random_double = (rand() / (double) RAND_MAX) / 1000.0;
      projectile.phy.vel_z += random_double * weapon.spray - weapon.spray / 2000.0;
      phy_rel_move(&projectile.phy, client->map, 0, projectile.harm_radius + 0.2, projectile.bounce);
      sprite_create(client->sprites, &projectile);
    }
    client->shot_timer = weapon.repeat_rate;
  }
}
void client_init(client_t* client, map_t* map, sprite_bank_t* sprites) {
  lfb_init(&client->lfb, 512, 288);
  client->map = map;
  client->sprites = sprites;
  caster_init(&client->caster, &client->lfb);
  input_init(&client->input);
  client_respawn(client);
}

int client_update(client_t* client, render_t render) {
  double move_speed = 0.01, rot_speed = 0.05, friction = 0.9;

  input_update(&client->input);
  client->camera.vel_x *= friction;
  client->camera.vel_y *= friction;
  if (input_is_pressed(&client->input, INPUT_FORWARD)) {
    client->camera.vel_x += client->camera.dir_x * move_speed;
    client->camera.vel_y += client->camera.dir_y * move_speed;
  }
  if (input_is_pressed(&client->input, INPUT_BACK)) {
    client->camera.vel_x -= client->camera.dir_x * move_speed;
    client->camera.vel_y -= client->camera.dir_y * move_speed;
  }
  if (input_is_pressed(&client->input, INPUT_RIGHT)) {
    client->camera.vel_x -= client->camera.dir_y * move_speed;
    client->camera.vel_y += client->camera.dir_x * move_speed;
  }
  if (input_is_pressed(&client->input, INPUT_LEFT)) {
    client->camera.vel_x += client->camera.dir_y * move_speed;
    client->camera.vel_y -= client->camera.dir_x * move_speed;
  }
  if (input_is_pressed(&client->input, INPUT_ROTATE_RIGHT)) {
    phy_rotate(&client->camera, rot_speed);
    phy_rotate(&client->camera_plane, rot_speed);
  }
  if (input_is_pressed(&client->input, INPUT_ROTATE_LEFT)) {
    phy_rotate(&client->camera, -rot_speed);
    phy_rotate(&client->camera_plane, -rot_speed);
  }
  if (input_is_pressed(&client->input, INPUT_SHOOT)) {
    client_shoot(client);
  }
  if (client->shot_timer > 0) {
    client->shot_timer--;
  }

  phy_rel_move(&client->camera, client->map, 0, 1, 0);

  caster_draw_map(&client->caster, client->map, &client->camera, &client->camera_plane);
  caster_draw_sprites(&client->caster, client->sprites, &client->camera, &client->camera_plane);
  render_update(render, &client->lfb);
  return input_is_pressed(&client->input, INPUT_EXIT);
}

void client_cleanup(client_t* client) {
  input_cleanup(&client->input);
  lfb_cleanup(&client->lfb);
  caster_cleanup(&client->caster);
}

