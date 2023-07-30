#include "map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void map_init(map_t* map, int width, int height, cell_t default_cell) {
  int i, cell_cnt;

  map->width = width;
  map->height = height;
  map->default_cell = default_cell;
  cell_cnt = width * height;
  map->cells = (cell_t*) malloc(cell_cnt * sizeof(cell_t));
  for (i = 0; i < cell_cnt; i++) {
    map->cells[i] = default_cell;
  }
}

void map_cleanup(map_t* map) {
  free(map->cells);
}

void map_set_cell(map_t* map, int x, int y, cell_t cell) {
  if (x < 0 || x >= map->width || y < 0 || y >= map->height) {
    return;
  }
  map->cells[x + y * map->width] = cell;
}

cell_t map_get_cell(map_t* map, int x, int y) {
  if (x < 0 || x >= map->width || y < 0 || y >= map->height) {
    return map->default_cell;
  }
  return map->cells[x + y * map->width];
}

uint8_t* map_to_packet(map_t* map, int* size) {
  int packet_size;
  uint8_t* packet;
  uint8_t* ptr;

  packet_size = 2 * sizeof(uint32_t) + (1 + map->width * map->height) * sizeof(cell_t);
  packet = malloc(packet_size);
  ptr = packet;
  memcpy(ptr, &map->width, sizeof(uint32_t));
  ptr += 4;
  memcpy(ptr, &map->height, sizeof(uint32_t));
  ptr += 4;
  memcpy(ptr, &map->default_cell, sizeof(cell_t));
  ptr += sizeof(cell_t);
  memcpy(ptr, map->cells, map->width * map->height * sizeof(cell_t));
  ptr += map->width * map->height * sizeof(cell_t);

  *size = packet_size;
  return packet;
}

void map_cleanup_packet(uint8_t* packet) {
  free(packet);
}

void map_from_packet(map_t* map, uint8_t* packet, int size) {
  uint8_t* ptr;
  int expected_size;
  if (size < 8) {
    fprintf(stderr, "map_from_packet(): size == %d (<8)\n", size);
    return;
  }
  ptr = packet;
  map->width = 0;
  memcpy(&map->width, ptr, sizeof(uint32_t));
  ptr += 4;
  map->height = 0;
  memcpy(&map->height, ptr, sizeof(uint32_t));
  ptr += 4;
  expected_size = 2 * sizeof(uint32_t) + (1 + map->width * map->height) * sizeof(cell_t);
  if (size != expected_size) {
    return;
  }
  memcpy(&map->default_cell, ptr, sizeof(cell_t));
  ptr += sizeof(cell_t);
  map->cells = malloc(map->width * map->height * sizeof(cell_t));
  memcpy(map->cells, ptr, map->width * map->height * sizeof(cell_t));
  ptr += map->width * map->height * sizeof(cell_t);
}
