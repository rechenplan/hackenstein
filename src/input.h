#ifndef INPUT_H
#define INPUT_H

#include <stdint.h>

#define INPUT_FORWARD      (0)
#define INPUT_BACK         (1)
#define INPUT_LEFT         (2)
#define INPUT_RIGHT        (3)
#define INPUT_ROTATE_RIGHT (4)
#define INPUT_ROTATE_LEFT  (5)
#define INPUT_SHOOT        (6)
#define INPUT_EXIT         (7)
#define INPUT_MAX          (8)

typedef void (*input_cb_t)(int);

typedef struct _input_t {
  uint32_t state;
  input_cb_t callback[INPUT_MAX];
} input_t;

void input_init(input_t* input);
void input_update(input_t* input);
void input_cleanup(input_t* input);
void input_register_onkey(input_t* input, int key, input_cb_t callback);
int input_is_pressed(input_t* input, int key);

#endif
