#ifndef NET_H
#define NET_H

#include "sprite.h"
#include "phy.h"
#include "player.h"
#include "input.h"
#include "caster.h"
#include "render.h"
#include "lfb.h"
#include "map.h"

typedef struct _net_t {
  int stuff;
} net_t;

void net_init(net_t* net, char* host, int port);
void net_update(net_t* net, player_t players[], sprite_bank_t* sprites, map_t* map);
void net_cleanup(net_t* net);

#endif
