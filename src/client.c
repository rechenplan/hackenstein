#include "client.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "phy.h"
#include <math.h>

void client_init(client_t* client, map_t* map) {
  lfb_init(&client->lfb, 512, 288);
  client->map = map;
  client->camera.pos_x = 8;
  client->camera.pos_y = 8;
  client->camera.dir_x = 1;
  client->camera.dir_y = 0;
  client->camera_plane.dir_x = 0;
  client->camera_plane.dir_y = 2.0 / 3.0;
  caster_init(&client->caster, &client->lfb);
  input_init(&client->input);
}

int client_update(client_t* client, render_t render) {
  double move_speed = 0.1, rot_speed = 0.1;

  input_update(&client->input);
  /* input */
  if (input_is_pressed(&client->input, INPUT_FORWARD)) {
    phy_rel_move(&client->camera, client->map, 0, move_speed);
  }
  if (input_is_pressed(&client->input, INPUT_BACK)) {
    phy_rel_move(&client->camera, client->map, 0, -move_speed);
  }
  if (input_is_pressed(&client->input, INPUT_RIGHT)) {
    phy_rel_move(&client->camera, client->map, move_speed, 0);
  }
  if (input_is_pressed(&client->input, INPUT_LEFT)) {
    phy_rel_move(&client->camera, client->map, -move_speed, 0);
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
  }
  caster_draw_map(&client->caster, client->map, &client->camera, &client->camera_plane);
  render_update(render, &client->lfb);
  return input_is_pressed(&client->input, INPUT_EXIT);
}

void client_cleanup(client_t* client) {
  input_cleanup(&client->input);
  lfb_cleanup(&client->lfb);
  caster_cleanup(&client->caster);
}

