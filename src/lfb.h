#ifndef LFB_H
#define LFB_H

#include <stdint.h>
#include <SDL2/SDL.h>

typedef uint32_t pixel_t;

typedef struct _lfb_t {
  pixel_t* buffer;
  SDL_Surface* surface;
  int width;
  int height;
} lfb_t;

void lfb_init(lfb_t* lfb, int width, int height);
void lfb_cleanup(lfb_t* lfb);
pixel_t *lfb_get_buffer(lfb_t* lfb);
void lfb_print_char(lfb_t* lfb, int c, int x, int y);
void lfb_print(lfb_t* lfb, const char* str, int x, int y);

#endif
