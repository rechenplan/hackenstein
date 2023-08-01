
#ifndef SPRITE_H
#define SPRITE_H

#include "lfb.h"
#include "map.h"
#include "phy.h"

#define COLLIION_NONE (0)
#define COLLISION_EXPLODE (1)

typedef struct _sprite_t {
  int active;
  int owner;
  phy_t physical;
  double height;
  double width;
  pixel_t color;
  float harm;
  double collision_radius;
  int collision_type;
  int boom;
  int max_boom;
  int exploding;
  int bounce;
} sprite_t;

typedef struct _sprite_bank_t {
  sprite_t* bank;
  int* order;
  double* distance;
  int size;
} sprite_bank_t;

void sprite_collide(sprite_t* sprite);
void sprite_init(sprite_bank_t* sprites, int size);
int sprite_create(sprite_bank_t* sprites, sprite_t* sprite);
void sprite_destroy(sprite_t* sprite);
sprite_t* sprite_get(sprite_bank_t* sprites, int i);
void sprite_update(sprite_bank_t* sprites, map_t* map, int elapsed_time);
void sprite_cleanup(sprite_bank_t* bank);
void sprite_sort_by_dist(sprite_bank_t* bank, vec3_t pos);
int sprite_is_used(sprite_t* sprite);

#endif
