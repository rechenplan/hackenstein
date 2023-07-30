#include "server.h"

int WinMain(HINSTANCE inst, HINSTANCE p_inst, LPSTR arg, int show) {
  player_t players[MAX_PLAYERS];

  ENetAddress address;
  ENetHost* server;
  ENetEvent event;

  printf("initializing enet...");
  if (enet_initialize() != 0) {
    printf("error\n");
    exit(1);
  } else {
    printf("ok\n");
  }
  address.host = ENET_HOST_ANY;
  address.port = 26000;
  printf("creating host...");
  server = enet_host_create(&address, MAX_PLAYERS, 16, 0, 0);
  if (!server) {
    printf("error\n");
    exit(1);
  } else {
    printf("ok\n");
  }
  printf("entering event loop.\n");
  while (enet_host_service(server, &event, 500) >= 0) {
    switch (event.type) {
      case ENET_EVENT_TYPE_CONNECT:
        printf("connection received.\n");
        break;
      case ENET_EVENT_TYPE_RECEIVE:
        enet_host_broadcast(server, 0, event.packet);
        break;
      case ENET_EVENT_TYPE_DISCONNECT:
        printf("disconnect received.\n");
        break;
    }
  }
  enet_host_destroy(server);
  enet_deinitialize();
  exit(0);
}
