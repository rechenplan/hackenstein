#define SDL
#ifdef SDL
#include <SDL2/SDL.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "mapcast.h"
#include "render.h"
#include "lfb.h"
#include "map.h"
#include "test.h"

#ifdef SDL
int WinMain() {
  SDL_Init(SDL_INIT_VIDEO);
#else
int main(int argc, char** argv) {
#endif
  test_map();
  test_lfb();
  test_render();
  test_mapcast();
#ifdef SDL
  SDL_Quit();
#endif
  return 0;
}

void test_mapcast() {
  camera_t cam;
  render_t render;
  lfb_t lfb;
  map_t map;
  int frame, i, j, t;

  printf("testing mapcast...");
  render = render_init();
  lfb_init(&lfb, 512, 288);
  map_init(&map, 16, 16, 1);
  for (j = 0; j < map.height; j++) {
    for (i = 0; i < map.width; i++) {
      map_set_cell(&map, i, j, 0);
    }
  }
  cam = mapcast_create_camera(0, 0, 1, 0);
#ifdef SDL
  t = SDL_GetTicks();
#else
  t = 0;
#endif
  for (frame = 0; frame < 500; frame++) {
    cam.pos_x += 0.01;
    mapcast_draw_map(&map, &lfb, &cam);
    render_update(render, &lfb);
  }
#ifdef SDL
  t = SDL_GetTicks() - t;
#endif
  map_cleanup(&map);
  lfb_cleanup(&lfb);
  render_cleanup(render);
  printf("ok\n");
  printf("render took %d ms per frame (%f fps)\n", t / 500, 1000.0 / (t / 500.0));
  return;
}

void test_render() {
  render_t render;
  lfb_t lfb;
  pixel_t* buffer;
  int buffer_size;
  int i, k, x, y, t;

  printf("testing render...");
  render = render_init();
  lfb_init(&lfb, 512, 288);
  buffer = lfb_get_buffer(&lfb);
  buffer_size = lfb.height * lfb.width;
  for (i = 0; i < buffer_size; i++) {
    x = i % lfb.width;
    y = i / lfb.width;
    buffer[i] = y ^ x;
  }

#ifdef SDL
  t = SDL_GetTicks();
#else
  t = 0;
#endif
  for (k = 0; k < 500; k++) {
    render_update(render, &lfb);
  }
#ifdef SDL
  t = SDL_GetTicks() - t;
#endif
  lfb_cleanup(&lfb);
  render_cleanup(render);
  printf("ok\n");
  printf("render took %d ms per frame (%f fps)\n", t / 500, 1000.0 / (t / 500.0));
}

void test_lfb() {
  lfb_t lfb;
  pixel_t* buffer;

  printf("testing lfb...");
  lfb_init(&lfb, 100, 200);
  assert(lfb.width == 100);
  assert(lfb.height == 200);
  buffer = lfb_get_buffer(&lfb);
  assert(buffer);
  lfb_cleanup(&lfb);
  printf("ok\n");
  return;
}

void test_map() {
  int i, j, size;
  map_t map[2];
  uint8_t* packet;

  printf("testing map...");
  map_init(&map[0], 10, 20, 0);
  assert(map[0].width == 10);
  assert(map[0].height == 20);
  for (i = 0; i < 10; i++) {
    for (j = 0; j < 20; j++) {
      map_set_cell(&map[0], i, j, 1);
      assert(map_get_cell(&map[0], i, j) == 1);
    }
  }
  map_set_cell(&map[0], 10, 20, 1);
  assert(map_get_cell(&map[0], 10, 20) == 0);
  packet = map_to_packet(&map[0], &size);
  map_from_packet(&map[1], packet, size);
  map_cleanup_packet(packet);
  assert(map[1].width == 10);
  assert(map[1].height == 20);
  assert(map[1].default_cell == 0);
  for (i = 0; i < 10; i++) {
    for (j = 0; j < 20; j++) {
      assert(map_get_cell(&map[1], 5, 5) == 1);
    }
  }
  map_cleanup(&map[0]);
  map_cleanup(&map[1]);
  printf("ok\n");
  return;
}
