#ifndef MOD_H
#define MOD_H

#include "player.h"
#include "object.h"

/* called when player and object collide */
void game_player_collide_with_object(player_t* player, object_t* object);

/* called every net frame for every player */
void game_player_update(player_t* player, object_bank_t* objects);

/* SHOULD be called on keydown */
void game_player_process_input(player_t* player, input_t* input, int elapsed_time);

/* called when new player is initialized */
void game_player_init(player_t* player);

/* called when player is destroyed */
void game_player_cleanup(player_t* player);

#endif
