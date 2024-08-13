#ifndef LAUNCHER_C_
#define LAUNCHER_C_

#include "shared.h"

typedef struct {
    int s_width, s_height;
    float centre_x;
    float screen_position_x, screen_position_y;
    float centre_y;
    float zoom;
    int max_iterations;
} FractalWindowCTX;

FractalWindowCTX *query_ctx(void);
/*
"*/

#endif