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
  int window_width, window_height, x, y;
  uint32_t* sdl_pixels;
  pixel_t* lfb_pixels;
  pixel_t lfb_color;
  int start_time, offset_x, offset_y, sdl_idx;

  start_time = SDL_GetTicks();
  window = (SDL_Window*) render;
  SDL_GetWindowSize(window, &window_width, &window_height);
  surface = SDL_GetWindowSurface(window);
  sdl_pixels = surface->pixels;
  lfb_pixels = lfb_get_buffer(lfb);
  SDL_LockSurface(surface);
  offset_x = (window_width - lfb->width) / 2;
  offset_y = (window_height - lfb->height) / 2;

  /* TODO: this loop slows the frame rate. speed it up. */
  for (y = 0; y < lfb->height; y++) {
    for (x = 0; x < lfb->width; x++) {
      lfb_color = lfb_pixels[x + y * lfb->width];
      sdl_idx = x + offset_x + (y + offset_y) * window_width;
      if (sdl_idx >= 0 && sdl_idx < window_width * window_height) {
        if (lfb_color == 0) {
          sdl_pixels[sdl_idx] = 0xffff0000;
        } else {
          sdl_pixels[sdl_idx] = lfb_color << 16 | lfb_color << 8 | lfb_color;
        }
      }
    }
  }
  SDL_UnlockSurface(surface);
  SDL_UpdateWindowSurface(window);
  return SDL_GetTicks() - start_time;
}
