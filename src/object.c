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
  object->exploding = 0;
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

static void object_explode(object_t* object) {
  object->exploding = 1;

  object->active = 0;
  object->physics.velocity.x = 0;
  object->physics.velocity.y = 0;
  object->physics.velocity.z = 0;
}

void object_collide(object_t* object) {
  switch (object->collision_type) {
    case COLLISION_EXPLODE:
      object_explode(object);
      break;
  }
}

void object_update(object_bank_t* objects, map_t* map, int elapsed_time) {
  int i;
  object_t* object;

  for (i = 0; i < objects->size; i++) {

    object = object_get(objects, i);
    if (object->exploding) {
      object->boom = object->boom + (object->max_boom - object->boom) / 8 + 1;
      if (object->boom >= object->max_boom) {
        object_destroy(object);
      }
    }

    if (!object->active) {
      continue;
    }

    /* if collision with wall */
    if (physics_update(&object->physics, map, object->bounces_left, object->height, elapsed_time)) {
      if (object->bounces_left) {
        object->bounces_left--;
      } else {
        object_collide(object);
      }
    }

  }
}

int object_is_used(object_t* object) {
  return (object->active || object->exploding);
}
