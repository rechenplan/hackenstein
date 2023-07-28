#ifndef CLIENT_H
#define CLIENT_H

#include "sprite.h"
#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "caster.h"
#include "phy.h"

typedef struct _client_t {
  render_t render;
  lfb_t    lfb;
  input_t  input;
  map_t*   map;
  sprite_bank_t* sprites;
  caster_t caster;
  phy_t    camera;
  phy_t    camera_plane;
} client_t;

void client_init(client_t* client, map_t* map, sprite_bank_t* sprites);
int client_update(client_t* client, render_t render);
void client_cleanup(client_t* client);

#endif
