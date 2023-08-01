#include "weapon.h"
#include "global.h"

weapon_t weapon_get(int i) {
  weapon_t weapon;
  memset(&weapon, 0, sizeof(weapon));
  switch (i) {
    case WEAPON_ROCKET:
      weapon.spray = 4.0;
      weapon.repeat_rate = 1000;
      weapon.projectile_count = 1;
      weapon.projectile_speed = 15.0;

      weapon.projectile.physics.friction = 0.999;
      weapon.projectile.physics.bouncy = 0;
      weapon.projectile.active = 1;
      weapon.projectile.height = 0.1;
      weapon.projectile.width = 0.1;
      weapon.projectile.color = GRAYSCALE(32);
      weapon.projectile.harm = 10;
      weapon.projectile.collision_radius = 2.0;
      weapon.projectile.boom = 0;
      weapon.projectile.max_boom = 100;
      weapon.projectile.bounces_left = 0;
      weapon.projectile.exploding = 0;
      weapon.projectile.type = OBJECT_TYPE_PROJECTILE;
      break;
    case WEAPON_SMG:
      weapon.spray = 4.0;
      weapon.repeat_rate = 100;
      weapon.projectile.physics.friction = 0.999;
      weapon.projectile.physics.bouncy = 0;
      weapon.projectile_count = 1;
      weapon.projectile.active = 1;
      weapon.projectile_speed = 15.0;
      weapon.projectile.height = 0.1;
      weapon.projectile.width = 0.1;
      weapon.projectile.color = GRAYSCALE(32);
      weapon.projectile.harm = 10;
      weapon.projectile.collision_radius = 2.0;
      weapon.projectile.boom = 0;
      weapon.projectile.max_boom = 100;
      weapon.projectile.bounces_left = 0;
      weapon.projectile.exploding = 0;
      weapon.projectile.type = OBJECT_TYPE_PROJECTILE;
      break;
    case WEAPON_MINE:
      weapon.spray = 1.0;
      weapon.repeat_rate = 500;
      weapon.projectile.physics.friction = 0.7;
      weapon.projectile.physics.bouncy = 0.5;
      weapon.projectile_count = 1;
      weapon.projectile.active = 1;
      weapon.projectile_speed = 1.0;
      weapon.projectile.height = 0.1;
      weapon.projectile.width = 0.2;
      weapon.projectile.color = GRAYSCALE(32);
      weapon.projectile.harm = 10;
      weapon.projectile.collision_radius = 2.0;
      weapon.projectile.boom = 0;
      weapon.projectile.max_boom = 100;
      weapon.projectile.bounces_left = 0;
      weapon.projectile.exploding = 0;
      weapon.projectile.type = OBJECT_TYPE_PROJECTILE;
      break;
    case WEAPON_RIFLE:
      weapon.spray = 1.0;
      weapon.repeat_rate = 500;
      weapon.projectile.physics.friction = 0.999;
      weapon.projectile.physics.bouncy = 0;
      weapon.projectile_count = 1;
      weapon.projectile.active = 1;
      weapon.projectile_speed = 15.0;
      weapon.projectile.height = 0.1;
      weapon.projectile.width = 0.1;
      weapon.projectile.color = GRAYSCALE(32);
      weapon.projectile.harm = 10;
      weapon.projectile.collision_radius = 2.0;
      weapon.projectile.boom = 0;
      weapon.projectile.max_boom = 100;
      weapon.projectile.bounces_left = 0;
      weapon.projectile.exploding = 0;
      weapon.projectile.type = OBJECT_TYPE_PROJECTILE;
      break;
    case WEAPON_SHOTGUN:
      weapon.spray = 15.0;
      weapon.repeat_rate = 800;
      weapon.projectile.physics.friction = 0.999;
      weapon.projectile.physics.bouncy = 1.0;
      weapon.projectile_count = 8;
      weapon.projectile.active = 1;
      weapon.projectile_speed = 15.0;
      weapon.projectile.height = 0.1;
      weapon.projectile.width = 0.1;
      weapon.projectile.color = GRAYSCALE(32);
      weapon.projectile.harm = 5;
      weapon.projectile.collision_radius = 0.5;
      weapon.projectile.boom = 0;
      weapon.projectile.max_boom = 5;
      weapon.projectile.bounces_left = 1;
      weapon.projectile.exploding = 0;
      weapon.projectile.type = OBJECT_TYPE_PROJECTILE;
      break;
  }
  return weapon;
}
