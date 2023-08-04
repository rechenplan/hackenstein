#include "hud.h"
#include "global.h"

void hud_init(hud_t* hud, int cross_hair_size) {
  hud->cross_hair_size = cross_hair_size;
  return;
}

void hud_update(hud_t* hud, int health, lfb_t* lfb, lfb_t* game) {
  int center_x, center_y, i;
  pixel_t* buffer;
  pixel_t* game_buffer;

  buffer = lfb_get_buffer(lfb);
  game_buffer = lfb_get_buffer(game);

  center_x = lfb->width / 2;
  center_y = lfb->height / 2;
  for (i = 0; i < hud->cross_hair_size; i++) {
    buffer[(center_x + i) + (center_y + 0) * lfb->width] = 0;
    buffer[(center_x - i) + (center_y + 0) * lfb->width] = 0;
    buffer[(center_x + 0) + (center_y + i) * lfb->width] = 0;
    buffer[(center_x + 0) + (center_y - i) * lfb->width] = 0;
  }

  for (i = 0; i < lfb->height * lfb->width; i++) {
    buffer[i] = game_buffer[i] ? game_buffer[i] : buffer[i];
  }
}

void hud_cleanup(hud_t* hud) {
  return;
}
