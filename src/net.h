#ifndef NET_H
#define NET_H

#include "player_struct.h"
#include "net_struct.h"
#include "game.h"

#define PLAYER_PACKET_MAX_SIZE (64)

net_t net_init(int listen_port);
void net_update(net_t net, player_t players[], map_t* map, int my_id, game_t* game, int current_time);
int net_connect(net_t net, const char* host, int port);
void net_broadcast(net_t net, const char* msg);
void net_cleanup(net_t net);

#endif
