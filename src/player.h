#ifndef PLAYER_H
#define PLAYER_H

#include "game.h"
#include "object.h"
#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "physics.h"
#include "global.h"
#include "player_struct.h"

/* per second */
#define PLAYER_FRICTION (1.0 / 100000.0)
#define PLAYER_BOUNCY (0.0)
#define PLAYER_MOVE_SPEED (40.0)
#define PLAYER_ROT_SPEED (TAU / 4)

#define SHARE_SHOOTING (0)
#define SHARE_HEALTH (1)

void player_init(player_t* player, object_bank_t* objects, int id);
void player_update(player_t* player, object_bank_t* objects, game_t* game, int elapsed_time);
int  player_process_input(player_t* player, input_t* input, game_t* game, int elapsed_time);
void player_net_interpolate(player_t* player, int elapsed_time);
void player_cleanup(player_t* player, game_t* game);

#endif
