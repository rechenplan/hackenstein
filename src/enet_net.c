#include <enet/enet.h>
#include <math.h>
#include "net.h"
#include "game.h"
#include "player.h"
#include "object.h"
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

void net_update(net_t net, player_t players[MAX_PLAYERS], map_t* map, int my_id, int current_time) {
  ENetEvent event;
  ENetPacket* packet;
  char* ptr;
  uint8_t id;
  uint8_t share_flag;
  enet_net_t* enet;
  char player_packet[PLAYER_PACKET_MAX_SIZE];
  int i;

  enet = (enet_net_t*) net;

  /* receive data from server */
  while (enet_host_service(enet->client, &event, 0) > 0) {
    if (event.type == ENET_EVENT_TYPE_RECEIVE && event.channelID == 0) {
      ptr = (char*) event.packet->data;
      id = *((uint8_t*) ptr); ptr++;
      if (id != my_id) {
        players[id].remote.packet_time = current_time;

        players[id].remote.last_position = players[id].object->physics.position;
        players[id].remote.last_rotation = players[id].object->physics.rotation;
        players[id].remote.interp = 0;

        players[id].remote.current_rotation = *((uint16_t*) ptr) * TAU / 65535; ptr += 2;
        players[id].remote.current_position.x = *((uint16_t*) ptr) * map->width / 65535.0; ptr += 2;
        players[id].remote.current_position.y = *((uint16_t*) ptr) * map->width / 65535.0; ptr += 2;
        players[id].remote.current_position.z = *((uint8_t*) ptr) / 255.0; ptr++;
        share_flag = *((uint8_t*) ptr); ptr++;
        i = 0;
        while (share_flag) {
          if (share_flag & 1) {
            players[id].share[i] = *((uint16_t*) ptr); ptr += 2;
          }
          share_flag >>= 1;
          i++;
        }
      }
      enet_packet_destroy(event.packet);
    }
  }

  /* send our data */
  ptr = player_packet;
  *((uint8_t*) ptr) = my_id; ptr++;

  *((uint16_t*) ptr) = players[my_id].object->physics.rotation * 65535 / TAU; ptr += 2;
  *((uint16_t*) ptr) = players[my_id].object->physics.position.x * 65535 / map->width; ptr += 2;
  *((uint16_t*) ptr) = players[my_id].object->physics.position.y * 65535 / map->height; ptr += 2;
  *((uint8_t*) ptr) = players[my_id].object->physics.position.z * 255; ptr++;
  *((uint8_t*) ptr) = players[my_id].share_flag; ptr++;

  i = 0;
  while (players[my_id].share_flag) {
    if (players[my_id].share_flag & 1) {
      *((uint16_t*) ptr) = players[my_id].share[i]; ptr += 2;
    }
    players[my_id].share_flag >>= 1;
    i++;
  }

  packet = enet_packet_create(player_packet, ptr - player_packet, 0);
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


