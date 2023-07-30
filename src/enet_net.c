#include <enet/enet.h>
#include "net.h"
#include "game.h"
#include "sprite.h"
#include "player.h"
#include "map.h"

#define NET_CHANNELS 2

typedef struct _enet_net_t {
  ENetHost* client;
  ENetPeer* peer;
} enet_net_t;

void* net_init(char* host, int port) {
  ENetAddress address;
  ENetEvent event;
  enet_net_t* net;

  net = (enet_net_t*) malloc(sizeof(enet_net_t));
  if (enet_initialize() != 0) {
    return NULL;
  }
  net->client = enet_host_create(NULL, 1, NET_CHANNELS, 0, 0);
  enet_address_set_host(&address, host);
  address.port = port;
  net->peer = enet_host_connect(net->client, &address, NET_CHANNELS, 0);
  if (!(enet_host_service(net->client, &event, 1000) > 0 && event.type == ENET_EVENT_TYPE_CONNECT)) {
    enet_deinitialize();
    return NULL;
  }
  return net;
}

void net_update(net_t net, player_t players[MAX_PLAYERS], int my_id) {
  ENetEvent event;
  ENetPacket* packet;
  player_t* player;
  enet_net_t* enet;
  int sprite;

  enet = (enet_net_t*) net;
  while (enet_host_service(enet->client, &event, 0) > 0) {
    if (event.type == ENET_EVENT_TYPE_RECEIVE && event.channelID == 0) {
      player = (player_t*) event.packet->data;
      /* TODO: this is just for testing. don't dump raw structs on the network. */
      if (player->id != my_id) {
        sprite = players[player->id].sprite;
        memcpy(&players[player->id], player, sizeof(player_t));
        players[player->id].sprite = sprite;
        enet_packet_destroy(event.packet);
      }
    }
  }
  packet = enet_packet_create(&players[my_id], sizeof(player_t), 0);
  if (packet) {
    enet_peer_send(enet->peer, 0, packet);
    enet_host_flush(enet->client);
  }
}

void net_cleanup(net_t net) {
  enet_net_t* enet;
  enet = (enet_net_t*) net;
  enet_host_destroy(enet->client);
  enet_deinitialize();
  free(enet);
}


