#ifndef GAME_H
#define GAME_H



#include "player_struct.h"
#include "object.h"
#include "map.h"
#include "net_struct.h"
#include "game_struct.h"

#define LUA_USE_C89

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

void game_init(game_t* game, object_t* me, map_t* map, net_t net, object_bank_t* objects);
void game_cleanup(game_t* game);

void game_on_tick(game_t* game, int elapsed_time);
void game_on_player_collide(game_t* game, object_t* object);
void game_on_key_down(game_t* game, int key);
void game_on_key_up(game_t* game, int key);
void game_on_receive(game_t* game, char* message);

#endif
