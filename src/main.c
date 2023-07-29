#include <SDL2/SDL.h>
#include "game.h"

int WinMain() {
  game_t game;
  SDL_Init(SDL_INIT_VIDEO);
  game_init(&game);
  while (game_update(&game) == 0) {}
  game_cleanup(&game);
  SDL_Quit();
  return 0;
}
