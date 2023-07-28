#ifndef SPRITE_H
#define SPRITE_H

#include "lfb.h"
#include "map.h"
#include "phy.h"

typedef struct _sprite_t {
  int active;
  phy_t phy;
  double vert;
  double vel;
  double height;
  double width;
  pixel_t color;
  int harm;
  int boom;
} sprite_t;

typedef struct _sprite_bank_t {
  sprite_t* bank;
  int* order;
  double* distance;
  int size;
} sprite_bank_t;

void sprite_init(sprite_bank_t* sprites, int size);
int sprite_create(sprite_bank_t* sprites, phy_t phy, double vel, double vert, double height, double width, pixel_t color, int harm);
void sprite_destroy(sprite_bank_t* sprites, int i);
sprite_t* sprite_get(sprite_bank_t* sprites, int i);
void sprite_update(sprite_bank_t* sprites, map_t* map);
void sprite_cleanup(sprite_bank_t* bank);
void sprite_sort_by_dist(sprite_bank_t* bank, phy_t *from);

#endif
