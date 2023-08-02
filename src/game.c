#include "game.h"
#include "player.h"
#include "object.h"
#include "net.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static int l_broadcast(lua_State* state) {
  const char* str;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  str = luaL_checkstring(state, 1);
  net_send_message(game->net, str);
  return 0;
}

static int l_map_set_cell(lua_State* state) {
  map_t* map;
  int x, y;
  cell_t cell;

  map = (map_t*) lua_touserdata(state, 1);
  x = luaL_checknumber(state, 2);
  y = luaL_checknumber(state, 3);
  cell = luaL_checknumber(state, 4);
  map_set_cell(map, x, y, cell);
  return 0;
}

static int l_map_get_cell(lua_State* state) {
  map_t* map;
  int x, y;

  map = (map_t*) lua_touserdata(state, 1);
  x = luaL_checknumber(state, 2);
  y = luaL_checknumber(state, 3);
  lua_pushnumber(state, map_get_cell(map, x, y));
  return 1;
}

void game_init(game_t* game, object_t* me, map_t* map, net_t net, object_bank_t* objects) {
  game->net = net;
  game->me = me;
  game->map = map;
  game->objects = objects;
  game->lua = luaL_newstate();
  luaL_openlibs(game->lua);
  if (luaL_dofile(game->lua, "scripts/init.lua") != LUA_OK) {
    printf("Error executing init.lua:\n%s\n", lua_tostring(game->lua, -1));
  }

  lua_pushcfunction(game->lua, l_broadcast);
  lua_setglobal(game->lua, "broadcast");

  lua_pushcfunction(game->lua, l_map_set_cell);
  lua_setglobal(game->lua, "map_set");

  lua_pushcfunction(game->lua, l_map_get_cell);
  lua_setglobal(game->lua, "map_get");

  lua_pushlightuserdata(game->lua, game);
  lua_setglobal(game->lua, "_GAME");
  game_on_load(game);
}

void game_cleanup(game_t* game) {
  lua_close(game->lua);
}

void game_on_load(game_t* game) {
  lua_getglobal(game->lua, "on_load");
  lua_pushlightuserdata(game->lua, game->me);
  lua_pushlightuserdata(game->lua, game->map);
  lua_pcall(game->lua, 2, 0, 0);
}

void game_on_collide(game_t* game, object_t* player, object_t* object) {
  lua_getglobal(game->lua, "on_collide");
  lua_pushlightuserdata(game->lua, player);
  lua_pushlightuserdata(game->lua, object);
  lua_pcall(game->lua, 2, 0, 0);
}

void game_on_key_down(game_t* game, int key) {
  lua_getglobal(game->lua, "on_key_down");
  lua_pushnumber(game->lua, key);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_on_key_up(game_t* game, int key) {
  lua_getglobal(game->lua, "on_key_up");
  lua_pushnumber(game->lua, key);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_on_join(game_t* game, object_t* player) {
  lua_getglobal(game->lua, "on_join");
  lua_pushlightuserdata(game->lua, player);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_on_leave(game_t* game, object_t* player) {
  lua_getglobal(game->lua, "on_leave");
  lua_pushlightuserdata(game->lua, player);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_on_receive(game_t* game, char* message) {
  lua_getglobal(game->lua, "on_receive");
  lua_pushstring(game->lua, message);
  lua_pcall(game->lua, 1, 0, 0);
}
