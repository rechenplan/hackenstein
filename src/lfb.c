#include "lfb.h"
#include <stdlib.h>

void lfb_init(lfb_t* lfb, int width, int height) {
  lfb->buffer = malloc(width * height * sizeof(pixel_t));
  lfb->width = width;
  lfb->height = height;
}

void lfb_cleanup(lfb_t* lfb) {
  free(lfb->buffer);
}

pixel_t* lfb_get_buffer(lfb_t* lfb) {
  return lfb->buffer;
}
