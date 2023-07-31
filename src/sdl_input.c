#include "input.h"
#include <SDL2/SDL.h>

void input_init(input_t* input) {
  input->state = 0;
}

void input_update(input_t* input) {
  SDL_Event event;

  while (SDL_PollEvent(&event)) {
    if (event.type == SDL_QUIT) {
      input->state |= (1 << INPUT_EXIT);
    }
    if (event.type == SDL_KEYDOWN) {
      switch (event.key.keysym.sym) {
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
        case SDLK_SPACE:
          input->state |= (1 << INPUT_SHOOT);
          break;
        case SDLK_ESCAPE:
          input->state |= (1 << INPUT_EXIT);
          break;
        case SDLK_o:
          input->state |= (1 << INPUT_CHANGE_GUN);
          break;
        case SDLK_TAB:
          input->state |= (1 << INPUT_CHANGE_SPEC);
          break;
      }
    }
    if (event.type == SDL_KEYUP) {
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
        case SDLK_SPACE:
          input->state &= ~(1 << INPUT_SHOOT);
          break;
        case SDLK_ESCAPE:
          input->state &= ~(1 << INPUT_EXIT);
          break;
        case SDLK_o:
          input->state &= ~(1 << INPUT_CHANGE_GUN);
          break;
        case SDLK_TAB:
          input->state &= ~(1 << INPUT_CHANGE_SPEC);
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
