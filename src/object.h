
#ifndef SPRITE_H
#define SPRITE_H

#include "lfb.h"
#include "map.h"
#include "physics.h"

#define OBJECT_TYPE_PLAYER     (0)
#define OBJECT_TYPE_PROJECTILE (1)
#define OBJECT_TYPE_PICKUP     (2)

typedef struct _object_t {

  /* belongs here */
  int                 active;
  struct _player_t*   owner;
  physics_t           physics;
  int                 next_respawn;
  int                 bounces_left;
  int                 boom;
  int                 exploding;

  /* belongs on blueprint */
  int                 type;
  double              height;
  double              width;
  pixel_t             color;
  double              collision_radius;
  int                 respawn_time;
  int                 max_boom;
  int                 max_bounces;

} object_t;

typedef struct _object_bank_t {
  object_t* bank;
  int*      order;
  double*   distance;
  int       size;
} object_bank_t;

void object_collide_with_wall(object_t* object);
void object_collide_with_player(object_t* object);
void object_init(object_bank_t* objects, int size);
object_t* object_create(object_bank_t* objects);
void object_destroy(object_t* object);
void object_update(object_bank_t* objects, map_t* map, int elapsed_time);
void object_cleanup(object_bank_t* bank);
void object_sort_by_dist(object_bank_t* bank, vec3_t pos);

int object_is_valid(object_t* object);
int object_is_exploding(object_t* object);
int object_is_respawning(object_t* object);

#endif
