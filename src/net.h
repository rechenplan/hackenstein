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

#define PLAYER_PACKET_SIZE (28)

typedef void* net_t;

net_t net_init(char* host, int port);
void net_update(net_t net, player_t players[], map_t* map, int my_id);
void net_cleanup(net_t net);

#endif
