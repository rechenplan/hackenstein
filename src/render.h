#ifndef RENDER_H
#define RENDER_H

#include "lfb.h"

typedef void* render_t;

render_t render_init();
void  render_cleanup(render_t render);
int  render_update(render_t render, lfb_t* lfb);

#endif
