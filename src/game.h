#ifndef MOD_H
#define MOD_H

#include "player.h"
#include "object.h"
#include "map.h"

typedef struct _game_data_t {
  uint8_t  weapon;
  int16_t  shot_timer;
  int16_t  swap_timer;
} game_data_t;

void game_map_load(map_t* map);

/* called when player and object collide */
void game_player_collide_with_object(player_t* player, object_t* object);

/* called every net frame for every player */
void game_player_update(player_t* player, object_bank_t* objects);

/* keyboard input */
void game_key_down(player_t* player, int key);
void game_key_up(player_t* player, int key);

/* called when new player is initialized */
void game_player_init(player_t* player, object_bank_t* objects);

/* called when player is destroyed */
void game_player_cleanup(player_t* player);

#endif
