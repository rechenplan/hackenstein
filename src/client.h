#ifndef CLIENT_H
#define CLIENT_H

#include "game.h"
#include "net.h"
#include "player.h"
#include "object.h"
#include "physics.h"
#include "input.h"
#include "caster.h"
#include "render.h"
#include "lfb.h"
#include "map.h"
#include "hud.h"
#include "global.h"

#define MAX_SPRITES (128)
#define LFB_WIDTH   (512)
#define LFB_HEIGHT  (288)

typedef struct _client_t {
  player_t players[MAX_PLAYERS];
  render_t render;
  map_t map;
  object_bank_t objects;
  input_t input;
  lfb_t lfb;
  caster_t caster;
  hud_t hud;
  int my_id;
  net_t net;
  game_t game;
  int start_time;
  int last_time;
  int net_frame;
  int physics_frame;
  int gfx_frame;
  int game_frame;
} client_t;

void client_init(client_t* client, char* host, int port, int my_id, int ms_time);
int client_update(client_t* client, int ms_time, int* sleep);
void client_cleanup(client_t* client);

#endif
