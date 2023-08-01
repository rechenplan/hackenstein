#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "object.h"
#include "lfb.h"

static object_t* object_get(object_bank_t* objects, int i) {
  return &objects->bank[i];
}

void object_init(object_bank_t* objects, int size) {
  objects->size = size;
  objects->bank = malloc(size * sizeof(object_t));
  objects->order = malloc(size * sizeof(int));
  objects->distance = malloc(size * sizeof(double));
  memset(objects->bank, 0, size * sizeof(object_t));
}

object_t* object_create(object_bank_t* objects) {
  int i;
  for (i = 0; i < objects->size; i++) {
    if (!object_is_used(&objects->bank[i]))
      break;
  }
  if (i == objects->size) {
    return NULL;
  }
  return &objects->bank[i];
}

void object_destroy(object_t* object) {
  object->active = 0;
  object->bullet.exploding = 0;
}

void object_cleanup(object_bank_t* objects) {
  free(objects->bank);
  free(objects->order);
  free(objects->distance);
}

object_bank_t* SORT_BANK;

static int object_dist_comp(const void *a, const void *b) {
  int i, j;

  i = *((int*) a);
  j = *((int*) b);
  return SORT_BANK->distance[i] < SORT_BANK->distance[j];
}

void object_sort_by_dist(object_bank_t* objects, vec3_t pos) {
  int i;
  object_t* object;

  for (i = 0; i < objects->size; i++) {
    object = object_get(objects, i);
    objects->order[i] = i;
    objects->distance[i] = (object->physics.position.x - pos.x) * (object->physics.position.x - pos.x) + (object->physics.position.y - pos.y) * (object->physics.position.y - pos.y);
  }
  SORT_BANK = objects;
  qsort(objects->order, objects->size, sizeof(int), object_dist_comp);
}

static void object_collide_projectile(object_t* projectile) {
  /* a projectile collided with player or a wall */
  projectile->bullet.exploding = 1;
  projectile->active = 0;
  projectile->physics.velocity.x = 0;
  projectile->physics.velocity.y = 0;
  projectile->physics.velocity.z = 0;
  return;
}

static void object_collide_player(object_t* player) {
  /* a player colliding with an object or a wall */
  return;
}

void object_collide(object_t* object) {
  switch (object->type) {
    case OBJECT_TYPE_PLAYER:
      object_collide_player(object);
      break;
    case OBJECT_TYPE_PROJECTILE:
      object_collide_projectile(object);
      break;
  }
}

void object_update(object_bank_t* objects, map_t* map, int elapsed_time) {
  int i;
  object_t* object;

  /* update all objects */
  for (i = 0; i < objects->size; i++) {

    object = object_get(objects, i);

    /* if exploding, adjust boom radius until it reaches max boom */
    if (object->type == OBJECT_TYPE_PROJECTILE && object->bullet.exploding) {
      object->bullet.boom = object->bullet.boom + (object->bullet.max_boom - object->bullet.boom) / 8 + 1;
      if (object->bullet.boom >= object->bullet.max_boom) {
        object_destroy(object);
      }
    }

    if (!object->active) {
      continue;
    }

    /* object / wall collisions (notice this includes player objects) */
    if (physics_update(&object->physics, map, object->bounces_left, object->height, elapsed_time)) {
      if (object->bounces_left > 0) {
        object->bounces_left--;
      } else if (!object->bounces_left) {
        object_collide(object);
      }
    }
  }
}

int object_is_used(object_t* object) {
  return (object->active || object->bullet.exploding);
}
