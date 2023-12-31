#include "game.h"
#include "player.h"
#include "object.h"
#include "net.h"
#include "lfb.h"

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

static int l_rect(lua_State* state) {
  game_t* game;
  int start_x, start_y, end_x, end_y;
  pixel_t col;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  start_x = luaL_checknumber(state, 1);
  start_y = luaL_checknumber(state, 2);
  end_x = luaL_checknumber(state, 3);
  end_y = luaL_checknumber(state, 4);
  col = luaL_checknumber(state, 5);
  lfb_rect(game->lfb, start_x, start_y, end_x, end_y, col);
  return 0;
}

static int l_printxy(lua_State* state) {
  game_t* game;
  const char* str;
  int x, y;
  pixel_t col;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  str = luaL_checkstring(state, 1);
  x = luaL_checknumber(state, 2);
  y = luaL_checknumber(state, 3);
  col = luaL_checknumber(state, 4);
  lfb_print(game->lfb, str, x, y, col);
  return 0;
}

static int l_cls(lua_State* state) {
  game_t* game;
  pixel_t* buffer;
  int i;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  buffer = lfb_get_buffer(game->lfb);
  for (i = 0; i < game->lfb->width * game->lfb->height; i++) {
    buffer[i] = 0;
  }
  return 0;
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

static int l_connect(lua_State* state) {
  const char* host;
  int port;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  host = luaL_checkstring(state, 1);
  port = luaL_checknumber(state, 2);
  net_connect(game->net, host, port);
  return 0;
}
static int l_broadcast(lua_State* state) {
  const char* str;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  str = luaL_checkstring(state, 1);
  net_broadcast(game->net, str);
  return 0;
}

static int l_set_map(lua_State* state) {
  int x, y, c;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  x = luaL_checknumber(state, 1);
  y = luaL_checknumber(state, 2);
  c = luaL_checknumber(state, 3);
  map_set_cell(game->map, x, y, c);
  return 0;
}

static int l_get_map(lua_State* state) {
  int x, y;
  game_t* game;

  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  x = luaL_checknumber(state, 1);
  y = luaL_checknumber(state, 2);
  lua_pushnumber(state, map_get_cell(game->map, x, y));
  return 1;
}

static int l_set_position(lua_State* state) {
  game_t* game;
  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  game->me->physics.position.x = get_field(state, "x");
  game->me->physics.position.y = get_field(state, "y");
  game->me->physics.position.z = get_field(state, "z");
  return 0;
}

static int l_get_position(lua_State* state) {
  game_t* game;
  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  lua_newtable(state);
  set_field(state, "x", game->me->physics.position.x);
  set_field(state, "y", game->me->physics.position.y);
  set_field(state, "z", game->me->physics.position.z);
  return 1;
}

static int l_set_velocity(lua_State* state) {
  game_t* game;
  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  game->me->physics.velocity.x = get_field(state, "x");
  game->me->physics.velocity.y = get_field(state, "y");
  game->me->physics.velocity.z = get_field(state, "z");
  return 0;
}

static int l_get_velocity(lua_State* state) {
  game_t* game;
  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pop(state, 1);
  lua_newtable(state);
  set_field(state, "x", game->me->physics.velocity.x);
  set_field(state, "y", game->me->physics.velocity.y);
  set_field(state, "z", game->me->physics.velocity.z);
  return 1;
}

static int l_set_rotation(lua_State* state) {
  game_t* game;
  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  game->me->physics.rotation = luaL_checknumber(state, 1);
  return 0;
}

static int l_get_rotation(lua_State* state) {
  game_t* game;
  lua_getglobal(state, "_GAME");
  game = (game_t*) lua_touserdata(state, -1);
  lua_pushnumber(state, game->me->physics.rotation);
  return 1;
}

void game_init(game_t* game, object_t* me, map_t* map, net_t net, lfb_t* lfb, object_bank_t* objects) {
  game->net = net;
  game->me = me;
  game->map = map;
  game->lfb = lfb;
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
  lua_pushcfunction(game->lua, l_connect);
  lua_setglobal(game->lua, "connect");
  lua_pushcfunction(game->lua, l_printxy);
  lua_setglobal(game->lua, "printxy");
  lua_pushcfunction(game->lua, l_rect);
  lua_setglobal(game->lua, "rect");
  lua_pushcfunction(game->lua, l_cls);
  lua_setglobal(game->lua, "cls");

  lua_pushlightuserdata(game->lua, game);
  lua_setglobal(game->lua, "_GAME");

  if (luaL_dofile(game->lua, "scripts/init.lua") != LUA_OK) {
    printf("error init.lua:\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_cleanup(game_t* game) {
  lua_close(game->lua);
}

void game_on_player_collide(game_t* game, object_t* object) {
  if (object->on_collision != LUA_NOREF) {
    lua_rawgeti(game->lua, LUA_REGISTRYINDEX, object->on_collision);
    if (lua_pcall(game->lua, 0, 0, 0) != LUA_OK) {
      printf("error on_player_collide():\n%s\n", lua_tostring(game->lua, -1));
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
  if (lua_pcall(game->lua, 1, 0, 0) != LUA_OK) {
    printf("error on_key_down():\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_on_key_up(game_t* game, int key) {
  lua_getglobal(game->lua, "on_key_up");
  lua_pushnumber(game->lua, key);
  if (lua_pcall(game->lua, 1, 0, 0) != LUA_OK) {
    printf("error on_key_up():\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_on_receive(game_t* game, int id, char* message) {
  lua_getglobal(game->lua, "on_receive");
  lua_pushnumber(game->lua, id);
  lua_pushstring(game->lua, message);
  if (lua_pcall(game->lua, 2, 0, 0) != LUA_OK) {
    printf("error on_receive():\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_on_connect(game_t* game, int id) {
  lua_getglobal(game->lua, "on_connect");
  lua_pushnumber(game->lua, id);
  if (lua_pcall(game->lua, 1, 0, 0) != LUA_OK) {
    printf("error on_connect():\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_on_disconnect(game_t* game, int id) {
  lua_getglobal(game->lua, "on_disconnect");
  lua_pushnumber(game->lua, id);
  if (lua_pcall(game->lua, 1, 0, 0) != LUA_OK) {
    printf("error on_disconnect():\n%s\n", lua_tostring(game->lua, -1));
  }
}

void game_on_tick(game_t* game, int elapsed_time) {
  lua_getglobal(game->lua, "on_tick");
  lua_pushnumber(game->lua, elapsed_time);
  if (lua_pcall(game->lua, 1, 0, 0) != LUA_OK) {
    printf("rrror on_tick():\n%s\n", lua_tostring(game->lua, -1));
  }
}
