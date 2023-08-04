#ifndef GLOBAL_H
#define GLOBAL_H

#define MAX_PLAYERS (128)
#define MAX_SPRITES (512)
#define LFB_WIDTH   (512)
#define LFB_HEIGHT  (288)

#define NET_FRAME_LIMIT (8)
#define PHY_FRAME_LIMIT (60)
#define GFX_FRAME_LIMIT (60)
#define GAME_FRAME_LIMIT (10)

#define GRAVITY (1.0)
#define TAU (3.14159 * 2)
#define GRAYSCALE(x) ((x << 16) | (x << 8) | (x))

#endif
