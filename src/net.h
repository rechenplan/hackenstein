#ifndef NET_H
#define NET_H

#include "player.h"
#include "object.h"
#include "physics.h"
#include "input.h"
#include "caster.h"
#include "render.h"
#include "lfb.h"
#include "map.h"

#define PLAYER_PACKET_MAX_SIZE (64)

typedef void* net_t;

net_t net_init(char* host, int port);
void net_update(net_t net, player_t players[], map_t* map, int my_id, int current_time);
void net_cleanup(net_t net);

#endif
