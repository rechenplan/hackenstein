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
  char* ptr;
  uint8_t id;
  enet_net_t* enet;
  int sprite;
  char player_packet[PLAYER_PACKET_SIZE];

  enet = (enet_net_t*) net;

  /* receive data from server */
  while (enet_host_service(enet->client, &event, 0) > 0) {
    if (event.type == ENET_EVENT_TYPE_RECEIVE && event.channelID == 0 && event.packet->dataLength == PLAYER_PACKET_SIZE) {
      ptr = (char*) event.packet->data;
      id = *((uint8_t*) ptr); ptr++;
      if (id != my_id) {
        players[id].weapon = *((uint8_t*) ptr); ptr++;
        players[id].health = *((int8_t*) ptr); ptr++;
        players[id].shooting = *((uint8_t*) ptr); ptr++;
        players[id].phy.position.x = *((float*) ptr); ptr += 4;
        players[id].phy.position.y = *((float*) ptr); ptr += 4;
        players[id].phy.position.z = *((float*) ptr); ptr += 4;
        players[id].phy.direction.x = *((float*) ptr); ptr += 4;
        players[id].phy.direction.y = *((float*) ptr); ptr += 4;
        players[id].phy.friction = *((float*) ptr); ptr += 4;
        players[id].phy.bouncy = *((float*) ptr); ptr += 4;
        /* TODO: reconstruct plane from phy.direction */
      }
      enet_packet_destroy(event.packet);
    }
  }

  /* send our data */
  ptr = player_packet;
  *((uint8_t*) ptr) = players[my_id].id; ptr++;
  *((uint8_t*) ptr) = players[my_id].weapon; ptr++;
  *((int8_t*) ptr) = players[my_id].health; ptr++;
  *((uint8_t*) ptr) = players[my_id].shooting; ptr++;
  *((float*) ptr) = players[my_id].phy.position.x; ptr += 4;
  *((float*) ptr) = players[my_id].phy.position.y; ptr += 4;
  *((float*) ptr) = players[my_id].phy.position.z; ptr += 4;
  *((float*) ptr) = players[my_id].phy.direction.x; ptr += 4;
  *((float*) ptr) = players[my_id].phy.direction.y; ptr += 4;
  *((float*) ptr) = players[my_id].phy.friction; ptr += 4;
  *((float*) ptr) = players[my_id].phy.bouncy; ptr += 4;
  packet = enet_packet_create(player_packet, PLAYER_PACKET_SIZE, 0);
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


