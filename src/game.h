#ifndef GAME_H
#define GAME_H

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
#include "global.h"

#define MAX_SPRITES (128)
#define LFB_WIDTH   (512)
#define LFB_HEIGHT  (288)

typedef struct _game_t {
  player_t players[MAX_PLAYERS];
  render_t render;
  map_t map;
  sprite_bank_t sprites;
  input_t input;
  lfb_t lfb;
  caster_t caster;
  int my_id;
  net_t net;
} game_t;

void game_init(game_t* game);
int game_update(game_t* game);
void game_cleanup(game_t* game);

#endif
