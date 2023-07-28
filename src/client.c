#include "client.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "render.h"
#include "phy.h"
#include <math.h>

void client_init(client_t* client) {
  int i, j;

  lfb_init(&client->lfb, 512, 288);
  caster_init(&client->caster, &client->lfb, 8, 8, 1, 0);
  input_init(&client->input);
  map_init(&client->map, 16, 16, 1);
  for (j = 0; j < client->map.height; j++) {
    for (i = 0; i < client->map.width; i++) {
      map_set_cell(&client->map, i, j, 0);
    }
  }
  client->render = render_init();
}

int client_update(client_t* client) {
  double move_speed = 0.1, rot_speed = 0.1;

  input_update(&client->input);
  if (input_is_pressed(&client->input, INPUT_FORWARD)) {
    phy_rel_move(&client->caster.camera, 0, move_speed);
  }
  if (input_is_pressed(&client->input, INPUT_BACK)) {
    phy_rel_move(&client->caster.camera, 0, -move_speed);
  }
  if (input_is_pressed(&client->input, INPUT_RIGHT)) {
    phy_rel_move(&client->caster.camera, move_speed, 0);
  }
  if (input_is_pressed(&client->input, INPUT_LEFT)) {
    phy_rel_move(&client->caster.camera, -move_speed, 0);
  }
  if (input_is_pressed(&client->input, INPUT_ROTATE_RIGHT)) {
    phy_rotate(&client->caster.camera, rot_speed);
    phy_rotate(&client->caster.camera_plane, rot_speed);
  }
  if (input_is_pressed(&client->input, INPUT_ROTATE_LEFT)) {
    phy_rotate(&client->caster.camera, -rot_speed);
    phy_rotate(&client->caster.camera_plane, -rot_speed);
  }
  if (input_is_pressed(&client->input, INPUT_SHOOT)) {
  }
  caster_draw_map(&client->caster, &client->map);
  render_update(client->render, &client->lfb);
  return input_is_pressed(&client->input, INPUT_EXIT);
}

void client_cleanup(client_t* client) {
  input_cleanup(&client->input);
  map_cleanup(&client->map);
  lfb_cleanup(&client->lfb);
  caster_cleanup(&client->caster);
  render_cleanup(client->render);
}

