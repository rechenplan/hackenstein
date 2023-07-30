#include <SDL2/SDL.h>
#include "game.h"
#include <windows.h>

int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR args, int show) {
  game_t game;
  char* host;
  char* port;
  char* id;
  int last_time, sleep;
  host = strtok(args, " ");
  port = strtok(NULL, " ");
  id = strtok(NULL, " ");
  SDL_Init(SDL_INIT_VIDEO);
  game_init(&game, host, atoi(port), atoi(id));
  last_time = SDL_GetTicks();
  while (game_update(&game) == 0) {
    sleep = 20 - (SDL_GetTicks() - last_time);
    if (sleep > 0) {
      SDL_Delay(sleep);
    }
    last_time = SDL_GetTicks();
  }
  game_cleanup(&game);
  SDL_Quit();
  return 0;
}
