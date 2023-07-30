#include "lfb.h"
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
