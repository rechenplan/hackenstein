#ifndef CLIENT_H
#define CLIENT_H

#include "render.h"
#include "lfb.h"
#include "input.h"
#include "map.h"
#include "caster.h"

typedef struct _client_t {
  render_t render;
  lfb_t    lfb;
  input_t  input;
  map_t    map;
  caster_t caster;
} client_t;

void client_init(client_t* client);
int client_update(client_t* client);
void client_cleanup(client_t* client);

#endif
