#include "weapon.h"

weapon_t weapon_get(int i) {
  weapon_t weapon;
  switch (i) {
    case WEAPON_ROCKET:
      weapon.spray = 12.0;
      weapon.repeat_rate = 120;
      weapon.proj.active = 1;
      weapon.proj.vert = 0.2;
      weapon.proj.vel = 0.25;
      weapon.proj.height = 0.1;
      weapon.proj.width = 0.1;
      weapon.proj.color = 48;
      weapon.proj.harm = 200;
      weapon.proj.harm_radius = 2;
      weapon.proj.boom = 0;
      weapon.proj.friction = 1.0;
      weapon.proj.bounce = 0;
      break;
    case WEAPON_SMG:
      weapon.spray = 12.0;
      weapon.repeat_rate = 5;
      weapon.proj.active = 1;
      weapon.proj.vert = 0.25;
      weapon.proj.vel = 0.3;
      weapon.proj.height = 0.1;
      weapon.proj.width = 0.1;
      weapon.proj.color = 64;
      weapon.proj.harm = 5;
      weapon.proj.harm_radius = 1;
      weapon.proj.boom = 0;
      weapon.proj.friction = 1.0;
      weapon.proj.bounce = 1;
      break;
    case WEAPON_MINE:
      weapon.spray = 1.0;
      weapon.repeat_rate = 60;
      weapon.proj.active = 1;
      weapon.proj.vert = 0.0;
      weapon.proj.vel = 0.5;
      weapon.proj.height = 0.1;
      weapon.proj.width = 0.2;
      weapon.proj.color = 190;
      weapon.proj.harm = 100;
      weapon.proj.harm_radius = 2;
      weapon.proj.boom = 0;
      weapon.proj.friction = 0.9;
      weapon.proj.bounce = 99;
      break;
    case WEAPON_RIFLE:
      weapon.spray = 2.0;
      weapon.repeat_rate = 90;
      weapon.proj.active = 1;
      weapon.proj.vert = 0.4;
      weapon.proj.vel = 0.4;
      weapon.proj.height = 0.1;
      weapon.proj.width = 0.1;
      weapon.proj.color = 190;
      weapon.proj.harm = 50;
      weapon.proj.harm_radius = 0.5;
      weapon.proj.boom = 0;
      weapon.proj.friction = 1.0;
      weapon.proj.bounce = 0;

  }
  return weapon;
}
