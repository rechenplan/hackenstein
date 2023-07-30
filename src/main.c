#include <SDL2/SDL.h>
#include "game.h"
#include "global.h"
#include <windows.h>

#define FPS_COUNTER_RESO (100)

int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR args, int show) {
  game_t game;
  char* host;
  char* port;
  char* id;
  int start_time, current_time, last_time, elapsed_time, frame;
  host = strtok(args, " ");
  port = strtok(NULL, " ");
  id = strtok(NULL, " ");
  SDL_Init(SDL_INIT_VIDEO);
  game_init(&game, host, atoi(port), atoi(id), start_time = SDL_GetTicks());
  frame = 0;
  last_time = start_time;
  while (game_update(&game, current_time = SDL_GetTicks()) == 0) {
    elapsed_time = current_time - last_time;
    if (elapsed_time < 1000 / FRAME_LIMIT) {
      SDL_Delay(1000 / FRAME_LIMIT - elapsed_time);
    }
    if (frame == FPS_COUNTER_RESO) {
      printf("%d fps\n", frame * 1000 / (current_time - start_time));
      frame = 0;
      start_time = current_time;
    } else {
      frame++;
    }
    last_time = current_time;
  }
  game_cleanup(&game);
  SDL_Quit();
  return 0;
}
