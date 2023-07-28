#include "caster.h"
#include "map.h"
#include "lfb.h"
#include <math.h>
#include <stdlib.h>


void caster_init(caster_t* caster, lfb_t* lfb, double pos_x, double pos_y, double dir_x, double dir_y) {
  caster->pos_x = pos_x;
  caster->pos_y = pos_y;
  caster->dir_x = dir_x;
  caster->dir_y = dir_y;
  caster->plane_x = 0;
  caster->plane_y = 2.0 / 3.0;
  caster->z_buffer = malloc(lfb->width * sizeof(double));
  caster->lfb = lfb;
  return;
}

void caster_cleanup(caster_t* caster) {
  free(caster->z_buffer);
}

void caster_draw_map(caster_t* caster, map_t* map) {
  pixel_t* buffer;
  int x, y, map_x, map_y, step_x, step_y, hit, side;
  int line_start, line_end, line_height;
  double camera_x, ray_x, ray_y;
  double side_dist_x, side_dist_y;
  double delta_dist_x, delta_dist_y;
  double perp_wall_dist;
  lfb_t* lfb;

  lfb = caster->lfb;
  buffer = lfb_get_buffer(lfb);
  for (x = 0; x < lfb->width; x++) {
    camera_x = 2.0 * x / lfb->width;
    ray_x = caster->dir_x + caster->plane_x * camera_x;
    ray_y = caster->dir_y + caster->plane_y * camera_x;
    map_x = (int) caster->pos_x;
    map_y = (int) caster->pos_y;
    delta_dist_x = fabs(1.0 / ray_x);
    delta_dist_y = fabs(1.0 / ray_y);
    if (ray_x < 0)
    {
      step_x = -1;
      side_dist_x = (caster->pos_x - map_x) * delta_dist_x;
    }
    else
    {
      step_x = 1;
      side_dist_x = (map_x + 1.0 - caster->pos_x) * delta_dist_x;
    }
    if (ray_y < 0)
    {
      step_y = -1;
      side_dist_y = (caster->pos_y - map_y) * delta_dist_y;
    }
    else
    {
      step_y = 1;
      side_dist_y = (map_y + 1.0 - caster->pos_y) * delta_dist_y;
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
    line_height = 1000 / perp_wall_dist;
    line_start = (lfb->height - line_height) / 2;
    line_end = (lfb->height + line_height) / 2;
    if (line_start < 0) {
      line_start = 0;
    }
    if (line_end > lfb->height - 1) {
      line_end = lfb->height - 1;
    }
    for (y = 0; y < line_start; y++) {
      buffer[x + y * lfb->width] = 0;
    }
    for (; y < line_end; y++) {
      buffer[x + y * lfb->width] = side ? 255 : 127;
    }
    for (; y < lfb->height; y++) {
      buffer[x + y * lfb->width] = 0;
    }
    caster->z_buffer[x] = perp_wall_dist;
  }
}
