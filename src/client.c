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
  client->camera_plane.dir_x = 0;
  client->camera_plane.dir_y = 2.0 / 3.0;
}

static void client_shoot(client_t* client) {
  sprite_t projectile;
  double random_double;
  weapon_t weapon;

  weapon = weapon_get(WEAPON_SMG);
  projectile = weapon.proj;
  projectile.phy = client->camera;

  /* debug */
  /*
    projectile.phy.pos_x = 8;
    projectile.phy.pos_y = 8;
    projectile.phy.dir_x = 1;
    projectile.phy.dir_y = 0;
  */
  if (!client->shot_timer) {
    random_double = (rand() / (double) RAND_MAX) / 100.0;
    projectile.phy.dir_x += random_double * weapon.spray;
    projectile.phy.dir_y += random_double * weapon.spray;
    projectile.vert += random_double * weapon.spray;
    phy_rel_move(&projectile.phy, client->map, 0, projectile.harm_radius + 0.25, projectile.bounce);
    sprite_create(client->sprites, &projectile);
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
  double move_speed = 0.1, rot_speed = 0.05;

  input_update(&client->input);
  if (input_is_pressed(&client->input, INPUT_FORWARD)) {
    phy_rel_move(&client->camera, client->map, 0, move_speed, 0);
  }
  if (input_is_pressed(&client->input, INPUT_BACK)) {
    phy_rel_move(&client->camera, client->map, 0, -move_speed, 0);
  }
  if (input_is_pressed(&client->input, INPUT_RIGHT)) {
    phy_rel_move(&client->camera, client->map, move_speed, 0, 0);
  }
  if (input_is_pressed(&client->input, INPUT_LEFT)) {
    phy_rel_move(&client->camera, client->map, -move_speed, 0, 0);
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

