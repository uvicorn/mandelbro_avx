#ifndef TYPES_HPP
#define TYPES_HPP

#include "stdint.h"

typedef struct {
    double x;
    double y;
} Point;

typedef struct {
    Point center;
    unsigned width;
    unsigned height;
    double scale;
} ScreenParams;

using PixelMatrix = uint8_t*;


#endif