#ifndef PIXELS_UPDATE_HPP
#define PIXELS_UPDATE_HPP

#include "types.hpp"
#include "stdint.h"

void ConstructMandelbrot_Slow(PixelMatrix pixels, const ScreenParams* screen);
void ConstructMandelbrot_AVX2(PixelMatrix pixels, const ScreenParams* screen);

#endif