
#ifndef SPRITE_H
#define SPRITE_H

#include "lfb.h"
#include "map.h"
#include "physics.h"

#define OBJECT_TYPE_PLAYER     (0)
#define OBJECT_TYPE_PROJECTILE (1)
#define OBJECT_TYPE_PICKUP     (2)

typedef struct _projectile_object_t {
  int boom;
  int max_boom;
  int exploding;
  int harm;
} projectile_object_t;

typedef struct _object_t {

  /* general object data */
  int                 active;
  struct _player_t*   owner;
  physics_t           physics;
  double              height;
  double              width;
  pixel_t             color;
  int                 bounces_left;
  double              collision_radius;

  /* type specific data */
  int                 type;
  projectile_object_t bullet;

} object_t;

typedef struct _object_bank_t {
  object_t* bank;
  int*      order;
  double*   distance;
  int       size;
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
