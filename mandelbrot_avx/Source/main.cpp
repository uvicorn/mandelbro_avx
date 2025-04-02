#include <SFML/Graphics.hpp>
#include "construct_mandelbrot.hpp"
#include "types.hpp"
#include "draw_mandelbrot.hpp"
#include <iostream>
#include "benchmarks.hpp"


#define RUN_BENCHMARK

int main(){

//#ifdef RUN_BENCHMARK
//    RunAllBenchmarks();
//    exit(0);
//#endif

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
        #ifdef __AVX2__ 
            ConstructMandelbrot_AVX2(pixels, screen);
        #else
            ConstructMandelbrot_Slow(pixels, screen);
        #endif

        texture.update(pixels);
        window.clear();
        window.draw(sprite);
        window.display();
    }

    free(pixels);

    return 0;
}