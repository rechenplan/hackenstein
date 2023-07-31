#include "hud.h"
#include "global.h"

void hud_init(hud_t* hud, int cross_hair_size) {
  hud->cross_hair_size = cross_hair_size;
  return;
}

void hud_update(hud_t* hud, int health, lfb_t* lfb) {
  int center_x, center_y, i, j, health_x;
  pixel_t* buffer;

  buffer = lfb_get_buffer(lfb);
  center_x = lfb->width / 2;
  center_y = lfb->height / 2;
  for (i = 0; i < hud->cross_hair_size; i++) {
    buffer[(center_x + i) + (center_y + 0) * lfb->width] = 0;
    buffer[(center_x - i) + (center_y + 0) * lfb->width] = 0;
    buffer[(center_x + 0) + (center_y + i) * lfb->width] = 0;
    buffer[(center_x + 0) + (center_y - i) * lfb->width] = 0;
  }
  health_x = (lfb->width - 100) / 2;
  for (j = lfb->height - 16; j < lfb->height - 8; j++) {
    for (i = 0; i < 100; i++) {
      if (i < health) {
        buffer[i + health_x + j * lfb->width] = GRAYSCALE(255);
      } else {
        buffer[i + health_x + j * lfb->width] = 0xffff0000;
      }
    }
  }
}

void hud_cleanup(hud_t* hud) {
  return;
}
