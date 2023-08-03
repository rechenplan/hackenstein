#include "lfb.h"
#include "font.h"
#include <stdlib.h>

void lfb_init(lfb_t* lfb, int width, int height) {
  lfb->surface = SDL_CreateRGBSurface(0, width, height, 32, 0, 0, 0, 0);
  lfb->buffer = (pixel_t*) lfb->surface->pixels;
  lfb->width = width;
  lfb->height = height;
}

void lfb_cleanup(lfb_t* lfb) {
  SDL_FreeSurface(lfb->surface);
}

pixel_t* lfb_get_buffer(lfb_t* lfb) {
  return lfb->buffer;
}

void lfb_print_char(lfb_t* lfb, int c, int x, int y) {
  int i, j, idx;
  pixel_t* buffer;

  buffer = lfb_get_buffer(lfb);
  for (j = 0; j < 8; j++) {
    for (i = 0; i < 8; i++) {
      if (x + i >= 0 && x + i < lfb->width && y + j >= 0 && y + j < lfb->height) {
        idx = x + i + (y + j) * lfb->width;
        buffer[idx] = font[c][j] & (1 << i) ? 0xffffffff : buffer[idx];
      }
    }
  }
}

void lfb_print(lfb_t* lfb, const char* str, int x, int y) {
  while (*str) {
    lfb_print_char(lfb, *str, x, y);
    str++;
    x += 8;
  }
}

