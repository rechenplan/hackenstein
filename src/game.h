#ifndef GAME_H
#define GAME_H

#include "player_struct.h"
#include "object.h"
#include "map.h"
#include "net_struct.h"

#define LUA_USE_C89

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct _game_t {
  player_t* local_player;
  net_t net;
  object_bank_t* objects;
  lua_State* lua;
} game_t;

void game_init(game_t* game, player_t* local_player, net_t net, object_bank_t* objects);
void game_cleanup(game_t* game);
void game_map_load(game_t* game, map_t* map);

void game_player_collide_with_object(game_t* game, player_t* player, object_t* object);
void game_key_down(game_t* game, int key);
void game_key_up(game_t* game, int key);
void game_player_init(game_t* game, player_t* player);
void game_player_cleanup(game_t* game, player_t* player);
void game_receive_message(game_t* game, char* message);

#endif
