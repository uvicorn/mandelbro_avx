#include "draw_mandelbrot.hpp"
#include "SFML/Graphics.hpp"
#include "types.hpp"
#include "consts.hpp"
#include <iostream>


//
// SCREEN PARAMS
//


ScreenParams* Screen_ctor(){
    ScreenParams* screen = (ScreenParams* )malloc(sizeof(ScreenParams));

    screen->center.x = DEFAULT_SCREEN_CENTER_X;
    screen->center.y = DEFAULT_SCREEN_CENTER_Y;

    screen->scale    = DEFAULT_ZOOM_SCALE;

    screen->height   = HEIGHT;
    screen->width    = WIDTH;

    return screen;
}

ScreenParams* Screen_dtor(ScreenParams* screen) {
    free(screen);
    return NULL;
}

//
// PIXEL MATRIX
//


PixelMatrix PixelMatrix_ctor(unsigned width, unsigned height) {
    PixelMatrix pixels = (PixelMatrix)malloc(width * height * PIXEL_SIZE * sizeof(uint8_t));

    // check aligment for AVX intrinsics
    assert(pixels != NULL);
    assert(pixels % 16 == 0);

    return pixels;
}

PixelMatrix PixelMatrix_dtor(PixelMatrix pixel_matrix){
    free(pixel_matrix);
    return NULL;
}


//
// WINDOW EVENT HANDLER
//

// movements on the screen use the following formulas:
// 
// screen_x = (real_x - x_center)  / scale + (W / 2)
// real_x   = (screen_x - (W / 2)) * scale + x_center


void HandleWindowEvent(sf::RenderWindow& window, sf::Texture& texture, ScreenParams* screen, PixelMatrix* pixels) {
    while (const std::optional event = window.pollEvent())
    {
        // Request for closing the window
        if (event->is<sf::Event::Closed>())
            window.close();

        // Window resize
        else if (const auto* resized = event->getIf<sf::Event::Resized>())
        {
            // TODO: fix center coords when resize windows

            screen->width  = resized->size.x;
            screen->height = resized->size.y;

            PixelMatrix_dtor(*pixels);
            *pixels = PixelMatrix_ctor(screen->width, screen->height);

            texture.resize({ screen->width, screen->height });
        }

        // Key pressed
        else if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        {
            using KeyPressed = sf::Keyboard::Scancode;

            switch (keyPressed->scancode) {
                case KeyPressed::Escape:
                    window.close();
                    break;

                case KeyPressed::Left:
                    screen->center.x -= PIXEL_SHIFT * screen->scale;
                    break;
                case KeyPressed::Right:
                    screen->center.x += PIXEL_SHIFT * screen->scale;
                    break;
                case KeyPressed::Up:
                    screen->center.y -= PIXEL_SHIFT * screen->scale;
                    break;
                case KeyPressed::Down:
                    screen->center.y += PIXEL_SHIFT * screen->scale;
                    break;

                case KeyPressed::Equal:
                case KeyPressed::NumpadPlus:
                    screen->scale /= ZOOM_SCALE_MULTIPLIER;
                    break;

                case KeyPressed::NumpadMinus:
                case KeyPressed::Hyphen:
                    screen->scale *= ZOOM_SCALE_MULTIPLIER;
                    break;

                default:
                    break;
            }
        }

        // Mouse scrolled
        else if (const auto* mouseWheelScrolled = event->getIf<sf::Event::MouseWheelScrolled>()) {
            switch (mouseWheelScrolled->wheel)
            {
                case sf::Mouse::Wheel::Vertical:
                    screen->center.y -= mouseWheelScrolled->delta * screen->scale * MOUSE_MOVEMENT_SPEED_SCALE;
                    break;
                case sf::Mouse::Wheel::Horizontal:
                    screen->center.x -= mouseWheelScrolled->delta * screen->scale * MOUSE_MOVEMENT_SPEED_SCALE;
                    break;
            }
        }

        // Mouse clicked
        if (const auto* mouseButtonPressed = event->getIf<sf::Event::MouseButtonPressed>())
        {
            if (mouseButtonPressed->button == sf::Mouse::Button::Right)
            {
                screen->scale *= ZOOM_SCALE_MULTIPLIER;
            }
            else if (mouseButtonPressed->button == sf::Mouse::Button::Left) {
                
                screen->center.x += (mouseButtonPressed->position.x - (double)screen->width  / 2) * MOUSE_SMOOTH_CLICK_MULTIPLIER * screen->scale;
                screen->center.y += (mouseButtonPressed->position.y - (double)screen->height / 2) * MOUSE_SMOOTH_CLICK_MULTIPLIER * screen->scale;

                screen->scale /= ZOOM_SCALE_MULTIPLIER;
            }
        }

        // Unknown event
        else {
            continue;
        }
    }
}