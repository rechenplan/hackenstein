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
    if (!sprites->bank[i].active)
      break;
  }
  if (i == sprites->size) {
    return -1;
  }
  memcpy(&sprites->bank[i], sprite, sizeof(sprite_t));
  return i;
}

void sprite_destroy(sprite_bank_t* sprites, int i) {
  sprites->bank[i].active = 0;
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
    sprites->distance[i] = (sprite->phy.position.x - pos.x) * (sprite->phy.position.x - pos.x) + (sprite->phy.position.y - pos.y) * (sprite->phy.position.y - pos.y);
  }
  SORT_BANK = sprites;
  qsort(sprites->order, sprites->size, sizeof(int), sprite_dist_comp);
}

void sprite_update(sprite_bank_t* sprites, map_t* map, int elapsed_time) {
  int i;
  sprite_t* sprite;

  for (i = 0; i < sprites->size; i++) {
    sprite = sprite_get(sprites, i);
    if (!sprite->active) {
      continue;
    }

    if (phy_update(&sprite->phy, map, sprite->bounce, sprite->height, elapsed_time) && sprite->harm) {
      if (sprite->bounce) {
        if (sprite->bounce > 0)
          sprite->bounce--;
      } else if (!sprite->boom) {
        sprite->boom = 1;
        sprite->phy.velocity.x = 0;
        sprite->phy.velocity.y = 0;
        sprite->phy.velocity.z = 0;
      }
    }
    if (sprite->boom) {
      sprite->boom = sprite->boom + (sprite->harm - sprite->boom) / 8 + 1;
      if (sprite->boom >= sprite->harm) {
        sprite->active = 0;
      }
    }
  }
}
