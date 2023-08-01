#ifndef WEAPON_H
#define WEAPON_H

#include "object.h"

typedef struct _weapon_t {
  object_t proj;
  double proj_speed;
  double spray;
  int repeat_rate;
  int proj_cnt;
  double bouncy;
  double friction;
} weapon_t;

#define WEAPON_ROCKET 0
#define WEAPON_SMG 1
#define WEAPON_MINE 2
#define WEAPON_RIFLE 3
#define WEAPON_SHOTGUN 4
#define MAX_WEAPON 5

weapon_t weapon_get(int i);

#endif
