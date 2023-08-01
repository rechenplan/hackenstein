#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "sprite.h"
#include "lfb.h"

void sprite_init(sprite_bank_t* sprites, int size) {
  sprites->size = size;
  sprites->bank = malloc(size * sizeof(sprite_t));
  sprites->order = malloc(size * sizeof(int));
  sprites->distance = malloc(size * sizeof(double));
  memset(sprites->bank, 0, size * sizeof(sprite_t));
}

int sprite_create(sprite_bank_t* sprites, sprite_t* sprite) {
  int i;
  for (i = 0; i < sprites->size; i++) {
    if (!sprite_is_used(&sprites->bank[i]))
      break;
  }
  if (i == sprites->size) {
    return -1;
  }
  memcpy(&sprites->bank[i], sprite, sizeof(sprite_t));
  return i;
}

void sprite_destroy(sprite_t* sprite) {
  sprite->active = 0;
  sprite->exploding = 0;
}

sprite_t* sprite_get(sprite_bank_t* sprites, int i) {
  return &sprites->bank[i];
}

void sprite_cleanup(sprite_bank_t* sprites) {
  free(sprites->bank);
  free(sprites->order);
  free(sprites->distance);
}

sprite_bank_t* SORT_BANK;

static int sprite_dist_comp(const void *a, const void *b) {
  int i, j;

  i = *((int*) a);
  j = *((int*) b);
  return SORT_BANK->distance[i] < SORT_BANK->distance[j];
}

void sprite_sort_by_dist(sprite_bank_t* sprites, vec3_t pos) {
  int i;
  sprite_t* sprite;

  for (i = 0; i < sprites->size; i++) {
    sprite = sprite_get(sprites, i);
    sprites->order[i] = i;
    sprites->distance[i] = (sprite->physical.position.x - pos.x) * (sprite->physical.position.x - pos.x) + (sprite->physical.position.y - pos.y) * (sprite->physical.position.y - pos.y);
  }
  SORT_BANK = sprites;
  qsort(sprites->order, sprites->size, sizeof(int), sprite_dist_comp);
}

static void sprite_explode(sprite_t* sprite) {
  sprite->active = 0;
  sprite->exploding = 1;
  sprite->physical.velocity.x = 0;
  sprite->physical.velocity.y = 0;
  sprite->physical.velocity.z = 0;
}

void sprite_collide(sprite_t* sprite) {
  switch (sprite->collision_type) {
    case COLLISION_EXPLODE:
      sprite_explode(sprite);
      break;
  }
}

void sprite_update(sprite_bank_t* sprites, map_t* map, int elapsed_time) {
  int i;
  sprite_t* sprite;

  for (i = 0; i < sprites->size; i++) {
    sprite = sprite_get(sprites, i);
    if (sprite->exploding) {
      sprite->boom = sprite->boom + (sprite->max_boom - sprite->boom) / 8 + 1;
      if (sprite->boom >= sprite->max_boom) {
        sprite_destroy(sprite);
      }
    }

    if (!sprite->active) {
      continue;
    }

    /* if collision with wall */
    if (phy_update(&sprite->physical, map, sprite->bounces_left, sprite->height, elapsed_time)) {
      if (sprite->bounces_left) {
        sprite->bounces_left--;
      } else {
        sprite_collide(sprite);
      }
    }

  }
}

int sprite_is_used(sprite_t* sprite) {
  return (sprite->active || sprite->exploding);
}
