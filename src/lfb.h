#ifndef LFB_H
#define LFB_H

#include <stdint.h>

typedef uint8_t pixel_t;

typedef struct _lfb_t {
  pixel_t* buffer;
  int width;
  int height;
} lfb_t;

void lfb_init(lfb_t* lfb, int width, int height);
void lfb_cleanup(lfb_t* lfb);
pixel_t *lfb_get_buffer(lfb_t* lfb);

#endif
