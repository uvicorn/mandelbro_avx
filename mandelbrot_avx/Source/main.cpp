#include <SFML/Graphics.hpp>
#include "construct_mandelbrot.hpp"
#include "types.hpp"
#include "draw_mandelbrot.hpp"
#include <iostream>


int main() {

    ScreenParams* screen = Screen_ctor();
    PixelMatrix pixels = PixelMatrix_ctor(screen->width, screen->height);

    // construct window
    sf::RenderWindow window(sf::VideoMode({ screen->width, screen->height }), "Mandelbrot Set");
    window.setFramerateLimit(60);

    sf::Texture texture(sf::Vector2u(screen->width, screen->height));
    sf::Sprite sprite(texture);

    // EVENT LOOP
    while (window.isOpen()) {
        HandleWindowEvent(window, texture, screen, &pixels);

        // Construct Mandelbrot set slowly/fastly

        // ConstructMandelbrot_Slow(pixels, screen);
        ConstructMandelbrot_AVX2(pixels, screen);

        texture.update(pixels);
        window.clear();
        window.draw(sprite);
        window.display();
    }

    free(pixels);

    return 0;
}