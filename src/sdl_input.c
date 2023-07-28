#include "input.h"
#include <SDL2/SDL.h>

void input_init(input_t* input) {
  int i;

  input->state = 0;
  for (i = 0; i < INPUT_MAX; i++) {
    input->callback[i] = 0;
  }
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
          if (input->callback[INPUT_FORWARD]) {
            input->callback[INPUT_FORWARD](INPUT_FORWARD);
          }
          break;
        case SDLK_a:
          input->state |= (1 << INPUT_LEFT);
          if (input->callback[INPUT_LEFT]) {
            input->callback[INPUT_LEFT](INPUT_LEFT);
          }
          break;
        case SDLK_s:
          input->state |= (1 << INPUT_BACK);
          if (input->callback[INPUT_BACK]) {
            input->callback[INPUT_BACK](INPUT_BACK);
          }
          break;
        case SDLK_d:
          input->state |= (1 << INPUT_RIGHT);
          if (input->callback[INPUT_RIGHT]) {
            input->callback[INPUT_RIGHT](INPUT_RIGHT);
          }
          break;
        case SDLK_k:
          input->state |= (1 << INPUT_ROTATE_LEFT);
          if (input->callback[INPUT_ROTATE_LEFT]) {
            input->callback[INPUT_ROTATE_LEFT](INPUT_ROTATE_LEFT);
          }
          break;
        case SDLK_SEMICOLON:
          input->state |= (1 << INPUT_ROTATE_RIGHT);
          if (input->callback[INPUT_ROTATE_RIGHT]) {
            input->callback[INPUT_ROTATE_RIGHT](INPUT_ROTATE_RIGHT);
          }
          break;
        case SDLK_SPACE:
          input->state |= (1 << INPUT_SHOOT);
          if (input->callback[INPUT_SHOOT]) {
            input->callback[INPUT_SHOOT](INPUT_SHOOT);
          }
          break;
        case SDLK_ESCAPE:
          input->state |= (1 << INPUT_EXIT);
          if (input->callback[INPUT_EXIT]) {
            input->callback[INPUT_EXIT](INPUT_EXIT);
          }
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
      }
    }
  }
}

void input_cleanup(input_t* input) {
  return;
}

void input_register_onkey(input_t* input, int key, input_cb_t callback) {
  input->callback[key] = callback;
}

int input_is_pressed(input_t* input, int key) {
  return input->state & (1 << key);
}
