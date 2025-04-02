#ifndef DRAW_MAN_HPP
#define DRAW_MAN_HPP

#include "stdint.h"
#include "types.hpp"
#include "SFML/Graphics.hpp"

void HandleWindowEvent(sf::RenderWindow& window, sf::Texture& texture, ScreenParams* screen, PixelMatrix* pixels);

PixelMatrix PixelMatrix_ctor(unsigned width, unsigned height);
PixelMatrix PixelMatrix_dtor(PixelMatrix pixel_matrix);


ScreenParams* Screen_ctor();
ScreenParams* Screen_dtor(ScreenParams* screen);

#endif