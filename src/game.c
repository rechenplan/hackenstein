#include "game.h"
#include "player.h"
#include "object.h"
#include "net.h"

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

static double get_field(lua_State* state, const char* key) {
  double result = 0;
  lua_pushstring(state, key);
  lua_gettable(state, -2);
  if (lua_isnumber(state, -1)) {
    result = lua_tonumber(state, -1);
  }
  lua_pop(state, 1);
  return result;
}

static void set_field(lua_State* state, const char* key, double value) {
  lua_pushstring(state, key);
  lua_pushnumber(state, value);
  lua_settable(state, -3);
}

/* spawn(blueprint, callback) */
static int l_spawn(lua_State* state) {
  object_t *object;
  game_t* game;
  int on_collision;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);

  on_collision = luaL_ref(state, LUA_REGISTRYINDEX);
  if (lua_istable(state, -1)) {

    object = object_create(game->objects);

    object->physics.position.x = get_field(state, "position_x");
    object->physics.position.y = get_field(state, "position_y");
    object->physics.position.z = get_field(state, "position_z");

    object->physics.velocity.x = get_field(state, "velocity_x");
    object->physics.velocity.y = get_field(state, "velocity_y");
    object->physics.velocity.z = get_field(state, "velocity_z");

    object->physics.rotation = get_field(state, "rotation");
    object->physics.friction = get_field(state, "friction");
    object->physics.bouncy = get_field(state, "bouncy");

    object->bounces_left = get_field(state, "bounces");
    object->height = get_field(state, "height");
    object->width = get_field(state, "width");
    object->collision_radius = get_field(state, "collision_radius");
    object->color = get_field(state, "color");
    object->max_boom = get_field(state, "explosion_size");

    object->owner = game->me->owner;
    object->is_player = 0;
    object->on_collision = on_collision;

  } else {

    luaL_unref(state, LUA_REGISTRYINDEX, on_collision);

  }
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

void game_on_player_collide(game_t* game, object_t* object) {
  if (object->on_collision != LUA_NOREF) {
    lua_rawgeti(game->lua, LUA_REGISTRYINDEX, object->on_collision);
    if (lua_pcall(game->lua, 0, 0, 0) != LUA_OK) {
      printf("Error executing collision callback:\n%s\n", lua_tostring(game->lua, -1));
    }
    /* this will be handled in object_destroy */
    /*
    luaL_unref(game->lua, LUA_REGISTRYINDEX, object->on_collision);
    object->on_collision = LUA_NOREF;
    */
  }
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
