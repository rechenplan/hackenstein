#include <SDL2/SDL.h>
#include "game.h"
#include <windows.h>

int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR args, int show) {
  game_t game;
  char* host;
  char* port;
  char* id;

  host = strtok(args, " ");
  port = strtok(NULL, " ");
  id = strtok(NULL, " ");
  SDL_Init(SDL_INIT_VIDEO);
  game_init(&game, host, atoi(port), atoi(id), SDL_GetTicks());
  while (game_update(&game, SDL_GetTicks()) == 0) { }
  game_cleanup(&game);
  SDL_Quit();
  return 0;
}
