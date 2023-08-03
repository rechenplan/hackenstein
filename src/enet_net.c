#include <enet/enet.h>
#include <math.h>
#include "client.h"
#include "player.h"
#include "object.h"
#include "map.h"
#include "game.h"
#include "net.h"

#define NET_CHANNELS 16

typedef struct _enet_net_t {
  ENetHost* client;
  ENetPeer* peer;
} enet_net_t;

void* net_init(int listen_port) {
  ENetAddress address;
  enet_net_t* net;

  net = (enet_net_t*) malloc(sizeof(enet_net_t));
  if (enet_initialize() != 0) {
    return NULL;
  }
  address.host = ENET_HOST_ANY;
  address.port = listen_port;
  net->client = enet_host_create(&address, MAX_PLAYERS, 2, 0, 0);
  if (!net->client) {
    free(net);
    return NULL;
  }
  return net;
}

void net_update(net_t net, player_t players[MAX_PLAYERS], map_t* map, int my_id, game_t* game, int current_time) {
  ENetEvent event;
  ENetPacket* packet;
  char* ptr;
  enet_net_t* enet;
  int i;
  player_t* player;
  char player_packet[PLAYER_PACKET_MAX_SIZE];

  enet = (enet_net_t*) net;

  /* receive data from server */
  while (enet_host_service(enet->client, &event, 0) > 0) {

    if (event.type == ENET_EVENT_TYPE_CONNECT) {
      event.peer->data = NULL;
      for (i = 0; i < MAX_PLAYERS; i++) {
        if (!players[i].connected) {
          players[i].connected = 1;
          event.peer->data = &players[i];
          break;
        }
      }
      if (i == MAX_PLAYERS) {
        enet_peer_reset(event.peer);
      }
    }

    if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
      player = (player_t*) event.peer->data;
      if (player) {
        player->connected = 0;
      }
    }

    if (event.type == ENET_EVENT_TYPE_RECEIVE) {

      /* position packet */
      if (event.channelID == 0) {
        ptr = (char*) event.packet->data;
        player = (player_t*) event.peer->data;
        if (player) {
          player->remote.packet_time = current_time;
          player->remote.last_position = player->object->physics.position;
          player->remote.last_rotation = player->object->physics.rotation;
          player->remote.interp = 0;

          player->remote.current_rotation = *((uint16_t*) ptr) * TAU / 65535; ptr += 2;
          player->remote.current_position.x = *((uint16_t*) ptr) * (map->width) / 65535.0; ptr += 2;
          player->remote.current_position.y = *((uint16_t*) ptr) * (map->height) / 65535.0; ptr += 2;
          player->remote.current_position.z = *((uint8_t*) ptr) / 255.0; ptr++;
        }
      }

      /* game packet */
      if (event.channelID == 1) {
        ptr = (char*) event.packet->data;
        game_on_receive(game, ptr);
      }

      enet_packet_destroy(event.packet);
    }
  }

  /* send location */
  ptr = player_packet;
  *((uint16_t*) ptr) = players[my_id].object->physics.rotation * 65535 / TAU; ptr += 2;
  *((uint16_t*) ptr) = players[my_id].object->physics.position.x * 65535 / (map->width); ptr += 2;
  *((uint16_t*) ptr) = players[my_id].object->physics.position.y * 65535 / (map->height); ptr += 2;
  *((uint8_t*) ptr) = players[my_id].object->physics.position.z * 255; ptr++;
  packet = enet_packet_create(player_packet, ptr - player_packet, 0);
  if (packet) {
    enet_host_broadcast(enet->client, 0, packet);
    enet_host_flush(enet->client);
  }
}

int net_connect(net_t net, const char* host, int port) {
  ENetAddress address;
  enet_net_t* enet;

  enet = (enet_net_t*) net;
  enet_address_set_host(&address, host);
  address.port = port;
  return enet_host_connect(enet->client, &address, 2, 0) != NULL;
}

void net_broadcast(net_t net, const char* msg) {
  ENetPacket* packet;
  enet_net_t* enet;

  enet = (enet_net_t*) net;
  packet = enet_packet_create(msg, strlen(msg) + 1, 0);
  if (packet) {
    enet_host_broadcast(enet->client, 1, packet);
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


