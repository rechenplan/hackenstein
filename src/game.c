#include "game.h"
#include "player.h"
#include "object.h"
#include "net.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/*

  create_object
  net_send_packet

*/

static void game_api_create_object(game_t* game, object_t* blueprint) {
  object_t* object;
  object = object_create(game->objects);
  memcpy(object, blueprint, sizeof(object_t));
  object->active = 1;
}

static void game_api_broadcast(game_t* game, unsigned char* buffer, int length) {
  net_send_packet(game->net, buffer, length);
}

void game_init(game_t* game, player_t* local_player, net_t net, object_bank_t* objects) {
  game->local_player = local_player;
  game->net = net;
  game->objects = objects;
  game->lua = luaL_newstate();
  luaL_openlibs(game->lua);
  if (luaL_dofile(game->lua, "scripts/init.lua") != LUA_OK) {
    printf("Error executing init.lua:\n%s", lua_tostring(game->lua, -1));
  }
}

void game_cleanup(game_t* game) {
  lua_close(game->lua);
}

void game_map_load(game_t* game, map_t* map) {
  lua_getglobal(game->lua, "map_load");
  memcpy(lua_newuserdata(game->lua, sizeof(map_t)), map, sizeof(map_t));
  lua_pcall(game->lua, 1, 0, 0);
}

void game_player_collide_with_object(game_t* game, player_t* player, object_t* object) {
  lua_getglobal(game->lua, "collision");
  memcpy(lua_newuserdata(game->lua, sizeof(player_t)), player, sizeof(player_t));
  memcpy(lua_newuserdata(game->lua, sizeof(object_t)), object, sizeof(object_t));
  lua_pcall(game->lua, 2, 0, 0);
}

void game_player_update(game_t* game, player_t* player, int elapsed_time) {
  lua_getglobal(game->lua, "update");
  memcpy(lua_newuserdata(game->lua, sizeof(player_t)), player, sizeof(player_t));
  lua_pushnumber(game->lua, elapsed_time);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_key_down(game_t* game, int key) {
  lua_getglobal(game->lua, "key_down");
  lua_pushnumber(game->lua, key);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_key_up(game_t* game, int key) {
  lua_getglobal(game->lua, "key_up");
  lua_pushnumber(game->lua, key);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_player_init(game_t* game, player_t* player) {
  lua_getglobal(game->lua, "player_init");
  memcpy(lua_newuserdata(game->lua, sizeof(player_t)), player, sizeof(player_t));
  lua_pcall(game->lua, 1, 0, 0);
}

void game_player_cleanup(game_t* game, player_t* player) {
  lua_getglobal(game->lua, "player_cleanup");
  memcpy(lua_newuserdata(game->lua, sizeof(player_t)), player, sizeof(player_t));
  lua_pcall(game->lua, 1, 0, 0);
}

void game_receive_packet(game_t* game, player_t* source, unsigned char* data, int data_len) {
}
