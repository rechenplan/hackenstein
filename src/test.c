#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "lfb.h"
#include "map.h"
#include "test.h"

int main(int argc, char** argv) {
  test_map();
  test_lfb();
  return 0;
}

void test_lfb() {
  lfb_t lfb;
  pixel_t* buffer;

  printf("testing lfb.c...");
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

  printf("testing map.c...");
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
