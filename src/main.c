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
  int sleep;
  int start_time, current_time, frame;
  host = strtok(args, " ");
  port = strtok(NULL, " ");
  id = strtok(NULL, " ");
  SDL_Init(SDL_INIT_VIDEO);
  game_init(&game, host, atoi(port), atoi(id), start_time = SDL_GetTicks());
  frame = 0;
  while (game_update(&game, current_time = SDL_GetTicks(), &sleep) == 0) {
    if (sleep) {
      SDL_Delay(1);
    }
    frame++;
    if (frame == FPS_COUNTER_RESO) {
      printf("%f fps\n", game.gfx_frame * 1000.0 / (current_time - start_time));
      frame = 0;
    }
  }
  game_cleanup(&game);
  SDL_Quit();
  return 0;
}
