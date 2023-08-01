#include "input.h"
#include <SDL2/SDL.h>

void input_init(input_t* input) {
  input->state = 0;
}

void input_update(input_t* input, char* key_down, char* key_up) {
  SDL_Event event;
  int key;

  memset(key_down, 0, INPUT_KEY_SIZE);
  memset(key_up, 0, INPUT_KEY_SIZE);

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      input->state |= (1 << INPUT_EXIT);
    }
    if (event.type == SDL_KEYDOWN) {
      key = event.key.keysym.sym;
      if (key >= 0 && key < INPUT_KEY_SIZE) {
        key_down[key] = 1;
      }
      switch (key) {
        case SDLK_w:
          input->state |= (1 << INPUT_FORWARD);
          break;
        case SDLK_a:
          input->state |= (1 << INPUT_LEFT);
          break;
        case SDLK_s:
          input->state |= (1 << INPUT_BACK);
          break;
        case SDLK_d:
          input->state |= (1 << INPUT_RIGHT);
          break;
        case SDLK_k:
          input->state |= (1 << INPUT_ROTATE_LEFT);
          break;
        case SDLK_SEMICOLON:
          input->state |= (1 << INPUT_ROTATE_RIGHT);
          break;
        case SDLK_ESCAPE:
          input->state |= (1 << INPUT_EXIT);
          break;
      }
    }
    if (event.type == SDL_KEYUP) {
      key = event.key.keysym.sym;
      if (key >= 0 && key < INPUT_KEY_SIZE) {
        key_up[key] = 1;
      }
      switch (event.key.keysym.sym) {
        case SDLK_w:
          input->state &= ~(1 << INPUT_FORWARD);
          break;
        case SDLK_a:
          input->state &= ~(1 << INPUT_LEFT);
          break;
        case SDLK_s:
          input->state &= ~(1 << INPUT_BACK);
          break;
        case SDLK_d:
          input->state &= ~(1 << INPUT_RIGHT);
          break;
        case SDLK_k:
          input->state &= ~(1 << INPUT_ROTATE_LEFT);
          break;
        case SDLK_SEMICOLON:
          input->state &= ~(1 << INPUT_ROTATE_RIGHT);
          break;
        case SDLK_ESCAPE:
          input->state &= ~(1 << INPUT_EXIT);
          break;
      }
    }
  }
}

void input_cleanup(input_t* input) {
  return;
}

int input_is_pressed(input_t* input, int key) {
  return input->state & (1 << key);
}
