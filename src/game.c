#include "game.h"
#include "player.h"
#include "object.h"
#include "net.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

/* spawn(blueprint, callback) */
static int l_spawn(lua_State* state) {
  object_t *object;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);

  /*
  get blueprint

  lua_gettable(state, 1);
  */

  object = object_create(game->objects);
  object->physics.position.x = 5;
  object->physics.position.y = 5;
  object->physics.position.z = 0;
  object->height = 0.5;
  object->width = 0.5;
  object->collision_radius = 1.0;
  object->color = GRAYSCALE(255);
  object->type = OBJECT_TYPE_PICKUP;
  object->on_collision = luaL_ref(state, LUA_REGISTRYINDEX);
  return 0;
}

static int l_broadcast(lua_State* state) {
  const char* str;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  str = luaL_checkstring(state, 1);
  net_send_message(game->net, str);
  return 0;
}

static int l_set_map(lua_State* state) {
  /* TODO: this should take a 2d array */
  /*
  map = (map_t*) lua_touserdata(state, 1);
  x = luaL_checknumber(state, 2);
  y = luaL_checknumber(state, 3);
  cell = luaL_checknumber(state, 4);
  map_set_cell(map, x, y, cell);
  */
  return 0;
}

static int l_get_map(lua_State* state) {
  /* TODO: this should return a 2d array */
  /*
  map = (map_t*) lua_touserdata(state, 1);
  x = luaL_checknumber(state, 2);
  y = luaL_checknumber(state, 3);
  lua_pushnumber(state, map_get_cell(map, x, y));
  */
  return 0;
}

static int l_set_position(lua_State* state) {
  return 0;
}

static int l_get_position(lua_State* state) {
  return 0;
}

static int l_set_velocity(lua_State* state) {
  return 0;
}

static int l_get_velocity(lua_State* state) {
  return 0;
}

static int l_set_rotation(lua_State* state) {
  return 0;
}

static int l_get_rotation(lua_State* state) {
  return 0;
}

void game_init(game_t* game, object_t* me, map_t* map, net_t net, object_bank_t* objects) {
  game->net = net;
  game->me = me;
  game->map = map;
  game->objects = objects;
  game->lua = luaL_newstate();
  luaL_openlibs(game->lua);

  lua_pushcfunction(game->lua, l_spawn);
  lua_setglobal(game->lua, "spawn");
  lua_pushcfunction(game->lua, l_broadcast);
  lua_setglobal(game->lua, "broadcast");
  lua_pushcfunction(game->lua, l_set_map);
  lua_setglobal(game->lua, "set_map");
  lua_pushcfunction(game->lua, l_get_map);
  lua_setglobal(game->lua, "get_map");
  lua_pushcfunction(game->lua, l_set_position);
  lua_setglobal(game->lua, "set_position");
  lua_pushcfunction(game->lua, l_get_position);
  lua_setglobal(game->lua, "get_position");
  lua_pushcfunction(game->lua, l_set_velocity);
  lua_setglobal(game->lua, "set_velocity");
  lua_pushcfunction(game->lua, l_get_velocity);
  lua_setglobal(game->lua, "get_velocity");
  lua_pushcfunction(game->lua, l_set_rotation);
  lua_setglobal(game->lua, "set_rotation");
  lua_pushcfunction(game->lua, l_get_rotation);
  lua_setglobal(game->lua, "get_rotation");

  lua_pushlightuserdata(game->lua, game);
  lua_setglobal(game->lua, "_GAME");

  if (luaL_dofile(game->lua, "scripts/init.lua") != LUA_OK) {
    printf("Error executing init.lua:\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_cleanup(game_t* game) {
  lua_close(game->lua);
}

/* TODO: need to call luaL_unref when being destroyed in general */
void game_on_collide(game_t* game, object_t* object) {
  lua_rawgeti(game->lua, LUA_REGISTRYINDEX, object->on_collision);
  if (lua_pcall(game->lua, 0, 0, 0) != LUA_OK) {
    printf("Error executing collision callback:\n%s\n", lua_tostring(game->lua, -1));
  }
  luaL_unref(game->lua, LUA_REGISTRYINDEX, object->on_collision);
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

void game_on_receive(game_t* game, char* message) {
  lua_getglobal(game->lua, "on_receive");
  lua_pushstring(game->lua, message);
  lua_pcall(game->lua, 1, 0, 0);
}

void game_on_tick(game_t* game, int elapsed_time) {
  lua_getglobal(game->lua, "on_tick");
  lua_pushnumber(game->lua, elapsed_time);
  lua_pcall(game->lua, 1, 0, 0);
}
