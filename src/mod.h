#ifndef MOD_H
#define MOD_H

#include "player.h"
#include "object.h"

void mod_player_collide_with_object(player_t* player, object_t* object);
void mod_player_update(player_t* player, object_bank_t* objects);
void mod_player_process_input(player_t* player, input_t* input, int elapsed_time);
void mod_player_init(player_t* player);
void mod_player_cleanup(player_t* player);

#endif

