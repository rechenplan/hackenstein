#ifndef MAP_H
#define MAP_H

#include <stdint.h>

typedef uint8_t cell_t;

typedef struct _map_t {
  cell_t *cells;
  cell_t default_cell;
  int width;
  int height;
} map_t;

void     map_init(map_t* map, int width, int height, cell_t default_cell);
void     map_cleanup(map_t* map);
void     map_set_cell(map_t* map, int x, int y, cell_t cell);
cell_t   map_get_cell(map_t* map, int x, int y);
uint8_t* map_to_packet(map_t* map, int* size);
void     map_cleanup_packet(uint8_t* packet);
void     map_from_packet(map_t* map, uint8_t* packet, int size);

#endif
