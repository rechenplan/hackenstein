#ifndef GAME_STRUCT_H
#define GAME_STRUCT_H

#include "player_struct.h"
#include "object.h"
#include "map.h"
#include "lfb.h"
#include "net_struct.h"

#define LUA_USE_C89

#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>

typedef struct _game_t {
  object_t* me;
  map_t* map;
  net_t net;
  lfb_t* lfb;
  object_bank_t* objects;
  lua_State* lua;
} game_t;

#endif
