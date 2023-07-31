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
#include "hud.h"
#include "net.h"

void game_init(game_t* game, char* host, int port, int my_id, int start_time) {
  int i, j;

  game->start_time = start_time;
  game->last_time = start_time;
  game->net_frame = 0;
  game->phy_frame = 0;
  game->gfx_frame = 0;
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
  hud_init(&game->hud, 8);
  player_respawn(&game->players[game->my_id], &game->sprites);
}

int game_update(game_t* game, int current_time, int *sleep) {
  int done, i, correct_net_frame, correct_phy_frame, correct_gfx_frame;
  int local;
  int frame_computed;
  player_t* myself;
  player_t* spec_player;

  myself = &game->players[game->my_id];
  spec_player = &game->players[myself->spec];
  frame_computed = 0;
  done = 0;

  /* net frame */
  correct_net_frame = (current_time - game->start_time) * NET_FRAME_LIMIT / 1000;
  if (game->net_frame <= correct_net_frame) {
    if (game->net) {
      net_update(game->net, game->players, &game->map, game->my_id, current_time);
    }
    for (i = 0; i < MAX_PLAYERS; i++) {
      player_shoot(&game->players[i], &game->sprites);
    }
    game->net_frame++;
    frame_computed = 1;
  }
  /* physics frame */
  correct_phy_frame = (current_time - game->start_time) * PHY_FRAME_LIMIT / 1000;
  if (game->phy_frame <= correct_phy_frame) {
    done = player_process_input(myself, &game->input, 1000 / PHY_FRAME_LIMIT);
    for (i = 0; i < MAX_PLAYERS; i++) {
      local = (i == game->my_id);
      player_update(&game->players[i], &game->sprites, &game->map, 1000 / PHY_FRAME_LIMIT, local);
    }
    sprite_update(&game->sprites, &game->map, 1000 / PHY_FRAME_LIMIT);
    game->phy_frame++;
    frame_computed = 1;
  }
  /* graphics frame */
  correct_gfx_frame = (current_time - game->start_time) * GFX_FRAME_LIMIT / 1000;
  if (game->gfx_frame <= correct_gfx_frame) {
    caster_update(&game->caster, &game->map, &game->sprites, spec_player);
    hud_update(&game->hud, spec_player, &game->lfb);
    render_update(game->render, &game->lfb);
    game->gfx_frame++;
    frame_computed = 1;
  }
  *sleep = !frame_computed;
  game->last_time = current_time;
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
  hud_cleanup(&game->hud);
  if (game->net) {
    net_cleanup(game->net);
  }
}
