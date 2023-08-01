
#ifndef SPRITE_H
#define SPRITE_H

#include "lfb.h"
#include "map.h"
#include "physics.h"

#define COLLISION_NONE (0)
#define COLLISION_EXPLODE (1)

typedef struct _object_t {
  int active;

  int owner;
  physics_t physics;
  double height;
  double width;
  pixel_t color;
  int bounces_left;
  double collision_radius;
  int is_player;

  int collision_type;
  int boom;
  int max_boom;
  int exploding;

  /* SCRIPTME */
  float harm;

} object_t;

typedef struct _object_bank_t {
  object_t* bank;
  int* order;
  double* distance;
  int size;
} object_bank_t;

void object_collide(object_t* object);
void object_init(object_bank_t* objects, int size);
object_t* object_create(object_bank_t* objects);
void object_destroy(object_t* object);
void object_update(object_bank_t* objects, map_t* map, int elapsed_time);
void object_cleanup(object_bank_t* bank);
void object_sort_by_dist(object_bank_t* bank, vec3_t pos);
int object_is_used(object_t* object);

#endif
