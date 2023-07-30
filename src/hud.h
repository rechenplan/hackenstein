#ifndef HUD_H
#define HUD_H

#include "player.h"
#include "lfb.h"

typedef struct _hud_t {
  int cross_hair_size;
} hud_t;

void hud_init(hud_t* hud, int cross_hair_size);
void hud_update(hud_t* hud, player_t* player, lfb_t* lfb);
void hud_cleanup(hud_t* hud);

#endif
