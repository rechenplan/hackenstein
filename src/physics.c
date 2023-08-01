#include "physics.h"
#include "global.h"
#include <math.h>

int physics_update(physics_t* physics, map_t* map, int bounce, double height, int elapsed_time) {
  double new_x, new_y, new_z, floor, ceil, time;
  int collision;

  time = elapsed_time / 1000.0;

  collision = 0;
  new_x = physics->position.x + physics->velocity.x * time;
  new_y = physics->position.y + physics->velocity.y * time;
  new_z = physics->position.z + physics->velocity.z * time;

  floor = height / 2;
  ceil = 1 - height / 2;

  if (new_z < floor) {
    physics->velocity.z = -physics->velocity.z * physics->bouncy;
    collision = 1;
    physics->position.z = floor;
  } else if (new_z > ceil) {
    physics->velocity.z = -physics->velocity.z * physics->bouncy;
    collision = 1;
    physics->position.z = ceil;
  } else {
    physics->position.z = new_z;
  }

  if (!map_get_cell(map, physics->position.x, new_y)) {
    physics->position.y = new_y;
  } else {
    physics->velocity.y = -physics->velocity.y * physics->bouncy;
    new_y = physics->position.y + physics->velocity.y * time;
    physics->position.y = new_y;
    collision = 1;
  }
  if (!map_get_cell(map, new_x, physics->position.y)) {
    physics->position.x = new_x;
  } else {
    physics->velocity.x = -physics->velocity.x * physics->bouncy;
    new_x = physics->position.x + physics->velocity.x * time;
    physics->position.x = new_x;
    collision = 1;
  }

  physics->velocity.x *= pow(physics->friction, time);
  physics->velocity.y *= pow(physics->friction, time);
  physics->velocity.z -= GRAVITY * time;

  return collision;
}

void physics_rotate(vec2_t* v, double rotation) {
  double dir_x, dir_y;

  dir_x = v->x;
  dir_y = v->y;
  v->x = dir_x * cos(rotation) - dir_y * sin(rotation);
  v->y = dir_x * sin(rotation) + dir_y * cos(rotation);
}
