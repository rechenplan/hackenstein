#define SDL
#ifdef SDL
#include <SDL2/SDL.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "sprite.h"
#include "phy.h"
#include "client.h"
#include "input.h"
#include "caster.h"
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
/*
  test_map();
  test_lfb();
  test_render();
  test_caster();
  test_input();
  test_sprite();
*/
  test_client();

#ifdef SDL
  SDL_Quit();
#endif
  return 0;
}

void test_sprite() {
  sprite_bank_t sprites;
  phy_t phy;
  int sprite_idx;

  printf("testing sprite...");
  phy.pos_x = 8;
  phy.pos_y = 8;
  sprite_init(&sprites, 10);
  sprite_idx = sprite_create(&sprites, phy, 1.0, 1.0, 64);
  sprite_sort_by_dist(&sprites, &phy);
  sprite_destroy(&sprites, sprite_idx);
  sprite_cleanup(&sprites);
  printf("ok\n");
  return;
}

void test_client() {
  client_t client;
  render_t render;
  map_t map;
  int i, j;
  sprite_bank_t sprites;
  phy_t phy;
  int sprite_idx, sprite_idx2;

  printf("testing client...");
  render = render_init();
  map_init(&map, 32, 32, 1);
  for (j = 0; j < map.height; j++) {
    for (i = 0; i < map.width; i++) {
      map_set_cell(&map, i, j, 0);
    }
  }
  map_set_cell(&map, 7, 7, 1);
  phy.pos_x = 16;
  phy.pos_y = 16;
  sprite_init(&sprites, 10);
  sprite_idx = sprite_create(&sprites, phy, 0.5, 1.0, 64);
  phy.pos_x = 8;
  sprite_idx2 = sprite_create(&sprites, phy, 0.5, 1.0, 96);
  client_init(&client, &map, &sprites);
  while (!client_update(&client, render)) {}
  client_cleanup(&client);
  map_cleanup(&map);
  sprite_destroy(&sprites, sprite_idx);
  sprite_destroy(&sprites, sprite_idx2);
  sprite_cleanup(&sprites);
  render_cleanup(render);
  printf("ok\n");
}

void test_input() {
  render_t render;
  lfb_t lfb;
  input_t input;
  pixel_t* buffer;
  int buffer_size;
  int i, done;

  printf("testing input...");
  render = render_init();
  lfb_init(&lfb, 512, 288);
  input_init(&input);
  buffer = lfb_get_buffer(&lfb);
  buffer_size = lfb.height * lfb.width;
  done = 0;
  while (!done) {
    render_update(render, &lfb);
    input_update(&input);
    for (i = 0; i < buffer_size; i++) {
      buffer[i] = input.state != 0 ? 255 : 0;
    }
    if (input_is_pressed(&input, INPUT_EXIT))
      done = 1;
  }
  lfb_cleanup(&lfb);
  input_cleanup(&input);
  render_cleanup(render);
  printf("ok\n");
}

void test_caster() {
  caster_t caster;
  render_t render;
  lfb_t lfb;
  map_t map;
  phy_t camera, camera_plane;
  int frame, i, j, t;

  printf("testing caster...");
  render = render_init();
  lfb_init(&lfb, 512, 288);
  map_init(&map, 16, 16, 1);
  for (j = 0; j < map.height; j++) {
    for (i = 0; i < map.width; i++) {
      map_set_cell(&map, i, j, 0);
    }
  }
  caster_init(&caster, &lfb);
  camera.pos_x = 8;
  camera.pos_y = 8;
  camera.dir_x = 1;
  camera.dir_y = 0;
  camera_plane.dir_x = 0;
  camera_plane.dir_y = 2.0 / 3.0;

#ifdef SDL
  t = SDL_GetTicks();
#else
  t = 0;
#endif
  for (frame = 0; frame < 500; frame++) {
    phy_rel_move(&camera, &map, 0, 0.01);
    caster_draw_map(&caster, &map, &camera, &camera_plane);
    render_update(render, &lfb);
  }
#ifdef SDL
  t = SDL_GetTicks() - t;
#endif
  map_cleanup(&map);
  lfb_cleanup(&lfb);
  caster_cleanup(&caster);
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
