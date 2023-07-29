#include "render.h"
#include <stdint.h>
#include <SDL2/SDL.h>

render_t render_init() {
  SDL_Window* window;

  window = SDL_CreateWindow(
    "hackenstein",
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    0,
    0,
    SDL_WINDOW_FULLSCREEN_DESKTOP);

  return window;
}

void render_cleanup(render_t render) {
  SDL_DestroyWindow((SDL_Window*) render);
}

int render_update(render_t render, lfb_t* lfb) {
  SDL_Window* window;
  SDL_Surface* surface;
  int window_width, window_height, x, y, i, j;
  uint32_t* sdl_pixels;
  pixel_t* lfb_pixels;
  pixel_t lfb_color;
  double scale_x, scale_y;
  int start_time;
  
  start_time = SDL_GetTicks();
  window = (SDL_Window*) render;
  SDL_GetWindowSize(window, &window_width, &window_height);
  surface = SDL_GetWindowSurface(window);
  sdl_pixels = surface->pixels;
  lfb_pixels = lfb_get_buffer(lfb);
  scale_x = (double) lfb->width / window_width;
  scale_y = (double) lfb->height / window_height;
  SDL_LockSurface(surface);
  /* TODO: this loop slows the frame rate. speed it up. */
  for (y = 0; y < window_height; y++) {
    for (x = 0; x < window_width; x++) {
      i = x * scale_x;
      j = y * scale_y;
      lfb_color = lfb_pixels[i + j * lfb->width];
      if (lfb_color == 0) {
        sdl_pixels[x + y * window_width] = 0xffff0000;
      } else {
        sdl_pixels[x + y * window_width] = lfb_color << 16 | lfb_color << 8 | lfb_color;
      }
    }
  }
  SDL_UnlockSurface(surface);
  SDL_UpdateWindowSurface(window);
  return SDL_GetTicks() - start_time;
}
