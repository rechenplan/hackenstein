#include "caster.h"
#include "map.h"
#include "lfb.h"
#include "sprite.h"
#include <math.h>
#include <stdlib.h>

#define WALL_HEIGHT 400.0

void caster_init(caster_t* caster, lfb_t* lfb) {
  caster->z_buffer = malloc(lfb->width * sizeof(double));
  caster->lfb = lfb;
  return;
}

void caster_cleanup(caster_t* caster) {
  free(caster->z_buffer);
}

void caster_update(caster_t* caster, map_t* map, sprite_bank_t* sprites, phy_t* camera, phy_t* camera_plane, int* hurt_me, int* abuser) {
    caster_draw_map(caster, map, camera, camera_plane);
    caster_draw_sprites(caster, sprites, camera, camera_plane, hurt_me, abuser);
}

void caster_draw_map(caster_t* caster, map_t* map, phy_t* camera, phy_t* camera_plane) {
  pixel_t* buffer;
  int x, y, map_x, map_y, step_x, step_y, hit, side;
  int line_start, line_end, line_height;
  double rel_x, ray_x, ray_y;
  double side_dist_x, side_dist_y;
  double delta_dist_x, delta_dist_y;
  double perp_wall_dist;
  lfb_t* lfb;

  lfb = caster->lfb;
  buffer = lfb_get_buffer(lfb);
  for (x = 0; x < lfb->width; x++) {
    rel_x = 2.0 * x / lfb->width - 1;
    ray_x = camera->dir_x + camera_plane->dir_x * rel_x;
    ray_y = camera->dir_y + camera_plane->dir_y * rel_x;
    map_x = (int) camera->pos_x;
    map_y = (int) camera->pos_y;
    delta_dist_x = ray_x == 0 ? 1e30 : fabs(1.0 / ray_x);
    delta_dist_y = ray_y == 0 ? 1e30 : fabs(1.0 / ray_y);
    if (ray_x < 0)
    {
      step_x = -1;
      side_dist_x = (camera->pos_x - map_x) * delta_dist_x;
    }
    else
    {
      step_x = 1;
      side_dist_x = (map_x + 1.0 - camera->pos_x) * delta_dist_x;
    }
    if (ray_y < 0)
    {
      step_y = -1;
      side_dist_y = (camera->pos_y - map_y) * delta_dist_y;
    }
    else
    {
      step_y = 1;
      side_dist_y = (map_y + 1.0 - camera->pos_y) * delta_dist_y;
    }
    hit = 0;
    while (!hit)
    {
      if (side_dist_x < side_dist_y)
      {
        side_dist_x += delta_dist_x;
        map_x += step_x;
        side = 0;
      }
      else
      {
        side_dist_y += delta_dist_y;
        map_y += step_y;
        side = 1;
      }
      if (map_get_cell(map, map_x, map_y)) {
        hit = 1;
      }
    }
    if (side == 0) {
      perp_wall_dist = side_dist_x - delta_dist_x;
    } else {
      perp_wall_dist = side_dist_y - delta_dist_y;
    }
    line_height = (int) (WALL_HEIGHT / perp_wall_dist);
    line_start = (lfb->height - line_height) / 2;
    line_end = (lfb->height + line_height) / 2;
    if (line_start < 0) {
      line_start = 0;
    }
    if (line_end < 0) {
      line_end = 0;
    }
    if (line_start > lfb->height - 1) {
      line_start = lfb->height - 1;
    }
    if (line_end > lfb->height - 1) {
      line_end = lfb->height - 1;
    }
    for (y = 0; y < line_start; y++) {
      buffer[x + y * lfb->width] = 1;
    }
    for (; y < line_end; y++) {
      buffer[x + y * lfb->width] = side ? 255 : 127;
    }
    for (; y < lfb->height; y++) {
      buffer[x + y * lfb->width] = 1;
    }
    caster->z_buffer[x] = perp_wall_dist;
  }
}

void caster_draw_sprites(caster_t* caster, sprite_bank_t* sprites, phy_t* camera, phy_t* camera_plane, int* hurt_me, int* abuser) {
  int i, x, y;
  lfb_t* lfb;
  pixel_t* buffer;
  sprite_t* sprite;
  double sprite_x, sprite_y, inv_det, transform_x, transform_y, sprite_size, screen_x, boom;
  int draw_start_x, draw_end_x, draw_start_y, draw_end_y;

  lfb = caster->lfb;
  buffer = lfb_get_buffer(lfb);
  sprite_sort_by_dist(sprites, camera, hurt_me, abuser);
  for (i = 0; i < sprites->size; i++) {
    sprite = sprite_get(sprites, sprites->order[i]);
    if (!sprite->active) {
      continue;
    }
    /* compute */
    sprite_x = sprite->phy.pos_x - camera->pos_x;
    sprite_y = sprite->phy.pos_y - camera->pos_y;
    inv_det = 1.0 / (camera_plane->dir_x * camera->dir_y - camera->dir_x * camera_plane->dir_y);
    transform_x = inv_det * (camera->dir_y * sprite_x - camera->dir_x * sprite_y);
    transform_y = inv_det * (camera_plane->dir_x * sprite_y - camera_plane->dir_y * sprite_x);
    screen_x = (lfb->width / 2) * (1 + transform_x / transform_y);
    sprite_size = WALL_HEIGHT / transform_y;
    boom = 1.0 + sprite->boom / 5.0;
    draw_end_x = screen_x + sprite->width * sprite_size * boom / 2;
    draw_start_x = screen_x - sprite->width * sprite_size * boom / 2;
    draw_end_y = (lfb->height + sprite->height * sprite_size * boom) / 2 - sprite_size * (sprite->phy.pos_z - 0.5);
    draw_start_y = (lfb->height - sprite->height * sprite_size * boom) / 2 - sprite_size * (sprite->phy.pos_z - 0.5);
    /* clamp */
    if (draw_end_x < 0) {
      draw_end_x = 0;
    }
    if (draw_end_x > lfb->width - 1) {
      draw_end_x = lfb->width - 1;
    }
    if (draw_end_y < 0) {
      draw_end_y = 0;
    }
    if (draw_end_y > lfb->height - 1) {
      draw_end_y = lfb->height - 1;
    }
    if (draw_start_x < 0) {
      draw_start_x = 0;
    }
    if (draw_start_x > lfb->width - 1) {
      draw_start_x = lfb->width - 1;
    }
    if (draw_start_y < 0) {
      draw_start_y = 0;
    }
    if (draw_start_y > lfb->height - 1) {
      draw_start_y = lfb->height - 1;
    }
    /* draw */
    for (y = draw_start_y; y < draw_end_y; y++) {
      for (x = draw_start_x; x < draw_end_x; x++) {
        if (transform_y > 0 && transform_y < caster->z_buffer[x] + 5) {
          buffer[x + y * lfb->width] = sprite->boom ? 0 : sprite->color;
        }
      }
    }
  }
}

