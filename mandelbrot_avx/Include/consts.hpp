#ifndef CONSTS_HPP
#define CONSTS_HPP

// default Screen width and height
#define WIDTH 1280
#define HEIGHT 720

#define DEFAULT_SCREEN_CENTER_X 0.0
#define DEFAULT_SCREEN_CENTER_Y 0.0
#define DEFAULT_ZOOM_SCALE 0.01


// mandelbrot set radius of convergence
#define RADIUS 100

// the number of iterations to figure out if a point falls into the mandelbrot set or not
#define ITERATIONS 256

// multiplier when zoom is increase/decrease
#define ZOOM_SCALE_MULTIPLIER 1.1

// step across the screen in pixels with keyboard click
#define PIXEL_SHIFT 15

// speed up mouse scrolling when moving across the screen
#define MOUSE_MOVEMENT_SPEED_SCALE 2.5

// constant to get more smooth jumps when moving across the screen with mouse clicks
#define MOUSE_SMOOTH_CLICK_MULTIPLIER 0.25

// number of iterations on each test case
#define BENCHMARK_ITERATIONS 5

// sizeof RGBA pixel in SFML pixels array
#define PIXEL_SIZE 4

#endif