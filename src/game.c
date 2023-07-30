#include "game.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sprite.h"
#include "phy.h"
#include "player.h"
#include "input.h"
#include "caster.h"
#include "render.h"
#include "lfb.h"
#include "map.h"
#include "net.h"

void game_init(game_t* game, char* host, int port, int my_id) {
  int i, j;

  game->my_id = my_id;
  game->render = render_init();
  game->net = net_init(host, port);
  map_init(&game->map, 32, 32, 1);
  for (j = 1; j < game->map.height; j++) {
    for (i = 1; i < game->map.width; i++) {
      map_set_cell(&game->map, i, j, 0);
    }
  }
  map_set_cell(&game->map, 7, 7, 1);
  input_init(&game->input);
  sprite_init(&game->sprites, MAX_SPRITES);
  for (i = 0; i < MAX_PLAYERS; i++) {
    player_init(&game->players[i], &game->sprites, i);
    /* TODO: remove this. it's for debugging */
    player_respawn(&game->players[i], &game->sprites);
  }
  lfb_init(&game->lfb, LFB_WIDTH, LFB_HEIGHT);
  caster_init(&game->caster, &game->lfb);
  player_respawn(&game->players[game->my_id], &game->sprites);
}

int game_update(game_t* game) {
  int done, i;
  player_t* myself;
  player_t* spec_player;

  myself = &game->players[game->my_id];
  spec_player = &game->players[myself->spec];
  done = player_process_input(myself, &game->input);
  if (game->net) {
    net_update(game->net, game->players, game->my_id);
  }
  for (i = 0; i < MAX_PLAYERS; i++) {
    player_update(&game->players[i], &game->sprites, &game->map);
  }
  sprite_update(&game->sprites, &game->map);
  caster_update(&game->caster, &game->map, &game->sprites, &spec_player->me, &spec_player->camera_plane);
  render_update(game->render, &game->lfb);
  return done;
}

void game_cleanup(game_t* game) {
  int i;

  render_cleanup(game->render);
  map_cleanup(&game->map);
  input_cleanup(&game->input);
  sprite_cleanup(&game->sprites);
  for (i = 0; i < MAX_PLAYERS; i++) {
    player_cleanup(&game->players[i]);
  }
  lfb_cleanup(&game->lfb);
  caster_cleanup(&game->caster);
  if (game->net) {
    net_cleanup(game->net);
  }
}
