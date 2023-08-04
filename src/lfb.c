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

void lfb_rect(lfb_t* lfb, int start_x, int start_y, int end_x, int end_y, pixel_t col) {
  int i, j;
  pixel_t* buffer;

  if (start_x < 0) {
    start_x = 0;
  }
  if (end_x < 0) {
    end_x = 0;
  }
  if (start_y < 0) {
    start_y = 0;
  }
  if (end_y < 0) {
    end_y = 0;
  }
  if (start_x > lfb->width - 1) {
    start_x = lfb->width - 1;
  }
  if (end_x > lfb->width - 1) {
    end_x = lfb->width - 1;
  }
  if (start_y > lfb->height - 1) {
    start_y = lfb->height - 1;
  }
  if (end_y > lfb->height - 1) {
    end_y = lfb->height - 1;
  }
  buffer = lfb_get_buffer(lfb);
  for (j = start_y; j < end_y; j++) {
    for (i = start_x; i < end_x; i++) {
      buffer[i + j * lfb->width] = col;
    }
  }
}

void lfb_print_char(lfb_t* lfb, int c, int x, int y, pixel_t col) {
  int i, j, idx;
  pixel_t* buffer;

  buffer = lfb_get_buffer(lfb);
  for (j = 0; j < 8; j++) {
    for (i = 0; i < 8; i++) {
      if (x + i >= 0 && x + i < lfb->width && y + j >= 0 && y + j < lfb->height) {
        idx = x + i + (y + j) * lfb->width;
        buffer[idx] = font[c][j] & (1 << i) ? col : buffer[idx];
      }
    }
  }
}

void lfb_print(lfb_t* lfb, const char* str, int x, int y, pixel_t col) {
  while (*str) {
    lfb_print_char(lfb, *str, x, y, col);
    str++;
    x += 8;
  }
}

