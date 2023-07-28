#ifndef SPRITE_H
#define SPRITE_H

#include "lfb.h"
#include "phy.h"

typedef struct _sprite_t {
  int active;
  phy_t phy;
  double height;
  double width;
  pixel_t color;
} sprite_t;

typedef struct _sprite_bank_t {
  sprite_t* bank;
  int* order;
  double* distance;
  int size;
} sprite_bank_t;

void sprite_init(sprite_bank_t* sprites, int size);
int sprite_create(sprite_bank_t* sprites, phy_t phy, double height, double width, pixel_t color);
void sprite_destroy(sprite_bank_t* sprites, int i);
sprite_t* sprite_get(sprite_bank_t* sprites, int i);
void sprite_cleanup(sprite_bank_t* bank);
void sprite_sort_by_dist(sprite_bank_t* bank, phy_t *from);

#endif
