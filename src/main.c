#include <SDL2/SDL.h>
#include "client.h"
#include "global.h"
#include <windows.h>

#define FPS_COUNTER_RESO (100)

int WinMain(HINSTANCE instance, HINSTANCE prev_instance, LPSTR args, int show) {
  client_t client;
  char* host;
  char* port;
  char* id;
  int sleep;
  int start_time, current_time, frame;
  host = strtok(args, " ");
  port = strtok(NULL, " ");
  id = strtok(NULL, " ");
  SDL_Init(SDL_INIT_VIDEO);
  client_init(&client, host, atoi(port), atoi(id), start_time = SDL_GetTicks());
  frame = 0;
  while (client_update(&client, current_time = SDL_GetTicks(), &sleep) == 0) {
    if (sleep) {
      SDL_Delay(1);
    }
    frame++;
    if (frame == FPS_COUNTER_RESO) {
      printf("%f fps\n", client.gfx_frame * 1000.0 / (current_time - start_time));
      frame = 0;
    }
  }
  client_cleanup(&client);
  SDL_Quit();
  return 0;
}
