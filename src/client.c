#include "client.h"
#include "game.h"
#include "net.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "physics.h"
#include "player.h"
#include "object.h"
#include "input.h"
#include "caster.h"
#include "render.h"
#include "lfb.h"
#include "map.h"
#include "hud.h"

void client_init(client_t* client, char* host, int port, int my_id, int start_time) {
  int i, j;

  client->start_time = start_time;
  client->last_time = start_time;
  client->net_frame = 0;
  client->my_id = my_id;
  client->physics_frame = 0;
  client->gfx_frame = 0;
  client->game_frame = 0;
  client->render = render_init();
  client->net = net_init(host, port);
  input_init(&client->input);
  object_init(&client->objects, MAX_SPRITES);
  lfb_init(&client->lfb, LFB_WIDTH, LFB_HEIGHT);
  caster_init(&client->caster, &client->lfb);
  hud_init(&client->hud, 8);
  map_init(&client->map, 32, 32, 1);
  for (j = 0; j < client->map.height; j++) {
    for (i = 0; i < client->map.width; i++) {
      map_set_cell(&client->map, i, j, 0);
    }
  }
  for (i = 0; i < MAX_PLAYERS; i++) {
    player_init(&client->players[i], &client->objects, i == my_id);
  }
  game_init(&client->game, client->players[my_id].object, &client->map, client->net, &client->objects);
}

int client_update(client_t* client, int current_time, int *sleep) {
  int done, i, correct_net_frame, correct_physics_frame, correct_gfx_frame, correct_game_frame;
  int frame_computed;
  player_t* myself;
  player_t* spec_player;

  myself = &client->players[client->my_id];
  spec_player = myself;
  frame_computed = 0;
  done = 0;

  /* game frame */
  correct_game_frame = (current_time - client->start_time) * GAME_FRAME_LIMIT / 1000;
  if (client->game_frame <= correct_game_frame) {
    game_on_tick(&client->game, 1000 / GAME_FRAME_LIMIT);
    client->game_frame++;
    frame_computed = 1;
  }

  /* net frame */
  correct_net_frame = (current_time - client->start_time) * NET_FRAME_LIMIT / 1000;
  if (client->net_frame <= correct_net_frame) {
    if (client->net) {
      net_update(client->net, client->players, &client->map, client->my_id, &client->game, current_time);
    }
    client->net_frame++;
    frame_computed = 1;
  }

  /* physics frame */
  correct_physics_frame = (current_time - client->start_time) * PHY_FRAME_LIMIT / 1000;
  if (client->physics_frame <= correct_physics_frame) {
    done = player_process_input(myself, &client->input, &client->game, 1000 / PHY_FRAME_LIMIT);
    for (i = 0; i < MAX_PLAYERS; i++) {
      player_update(&client->players[i], &client->objects, &client->game, 1000 / PHY_FRAME_LIMIT);
    }
    object_update(&client->objects, &client->map, &client->game, 1000 / PHY_FRAME_LIMIT);
    client->physics_frame++;
    frame_computed = 1;
  }

  /* graphics frame */
  correct_gfx_frame = (current_time - client->start_time) * GFX_FRAME_LIMIT / 1000;
  if (client->gfx_frame <= correct_gfx_frame) {
    caster_update(&client->caster, &client->map, &client->objects, spec_player);
    hud_update(&client->hud, 100, &client->lfb);
    render_update(client->render, &client->lfb);
    client->gfx_frame++;
    frame_computed = 1;
  }

  *sleep = !frame_computed;
  client->last_time = current_time;
  return done;
}

void client_cleanup(client_t* client) {
  int i;

  render_cleanup(client->render);
  map_cleanup(&client->map);
  input_cleanup(&client->input);
  object_cleanup(&client->objects);
  for (i = 0; i < MAX_PLAYERS; i++) {
    player_cleanup(&client->players[i], &client->game);
  }
  lfb_cleanup(&client->lfb);
  caster_cleanup(&client->caster);
  hud_cleanup(&client->hud);
  if (client->net) {
    net_cleanup(client->net);
  }
  game_cleanup(&client->game);
}
