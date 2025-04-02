#include "construct_mandelbrot.hpp"
#include <cstdlib>
#include "stdint.h"
#include "consts.hpp"
#include <immintrin.h>
#include "types.hpp"
#include <cassert>
#include "stdio.h"
#include <iostream>


#define ALL_ONE_BITS 0xFFFFFFFFFFFFFFFFull

void ConstructMandelbrot_AVX2(PixelMatrix pixels, const ScreenParams* screen) {
    static_assert(1 < ITERATIONS && ITERATIONS < (2 << 16));
    static_assert(ITERATIONS == 256);
    // check alignment
    assert(screen->height % 4 == 0 && screen->width % 4 == 0);
    assert(pixels % 16 == 0);


    const __m256d scaler = _mm256_set1_pd(screen->scale);

    __m256d x0_delta = _mm256_set1_pd(-(double)screen->width  / 2);
    __m256d y0_delta = _mm256_set1_pd(-(double)screen->height / 2);
    
    x0_delta = _mm256_mul_pd(x0_delta, scaler);
    y0_delta = _mm256_mul_pd(y0_delta, scaler);

    x0_delta = _mm256_add_pd(x0_delta, _mm256_set1_pd(screen->center.x));
    y0_delta = _mm256_add_pd(y0_delta, _mm256_set1_pd(screen->center.y));

    const __m256d radius_vector = _mm256_set1_pd(RADIUS);

    for (unsigned y_i = 0; y_i < screen->height; y_i++) {
        for (unsigned x_i = 0; x_i < screen->width; x_i += 4) {
            unsigned pixel_index = (y_i * screen->width + x_i) * 4;

            // x x x x
            __m256d x0 = _mm256_add_pd(_mm256_set_pd(0, 1, 2, 3), _mm256_set1_pd(x_i));
            __m256d y0 = _mm256_set1_pd(y_i);

            // x0 = (x_i + i)*scale - W*scale/2 + x_center

            x0 = _mm256_mul_pd(x0, scaler);
            y0 = _mm256_mul_pd(y0, scaler);

            x0 = _mm256_add_pd(x0, x0_delta);
            y0 = _mm256_add_pd(y0, y0_delta);

            //
            // unroll first iteration=0 
            // ================================>

            __m256d x = x0;
            __m256d y = y0;

            __m256d x2 = _mm256_mul_pd(x0, x0);
            __m256d y2 = _mm256_mul_pd(y0, y0);

            // <================================

            // save iterations for [x, x+1, x+2, x+3]
            __m256i iter_add_mask;
            __m256i iter_vector = _mm256_set1_epi64x(ALL_ONE_BITS);

            unsigned iteration = 1;

            while (iteration < ITERATIONS) {
                // true -> 0xffffff...; false -> 0x0000... 
                __m256d cmp_vector = _mm256_cmp_pd(_mm256_add_pd(x2, y2), radius_vector, _CMP_LE_OQ);

                if (_mm256_testz_pd(cmp_vector, cmp_vector)) {
                    break;
                }
                
                iter_add_mask = _mm256_castpd_si256(cmp_vector);
                iter_vector = _mm256_add_epi64(iter_vector, iter_add_mask);

                // y = 2 * x * y + y0
                y = _mm256_mul_pd(_mm256_mul_pd(x, y), _mm256_set1_pd(2));
                y = _mm256_add_pd(y, y0);
                
                // x = x2 - y2 + x0
                x = _mm256_add_pd(_mm256_sub_pd(x2, y2), x0);

                // x2 = x * x
                x2 = _mm256_mul_pd(x, x);
                y2 = _mm256_mul_pd(y, y);

                iteration++;
            }

            iter_vector = _mm256_xor_si256(iter_vector, _mm256_set1_epi64x(ALL_ONE_BITS));

            // store 64 bit blocks into last 128 bits
            // [0,a, 0,b, 0,c, 0,d] -> [0,0,0,0, a,b,c,d]
            iter_vector = _mm256_permutevar8x32_epi32(
                iter_vector,
                _mm256_setr_epi32(6, 4, 2, 0, 1, 1, 1, 1)
            );

            //
            // create result color
            //

            // ? ? ? 123 -> 123 123 123 0
            __m128i result_color = _mm_shuffle_epi8(
                _mm256_castsi256_si128(iter_vector), // cast m256i to m128i with only last 128 bits
                _mm_set_epi8(3, 12, 12, 12, 3, 8, 8, 8, 3, 4, 4, 4, 3, 0, 0, 0)
            );

            // 1 1 1 0 -> 1 1 1 255
            result_color = _mm_xor_si128(result_color, _mm_set1_epi32(255 << 8 * 3));

            // save result color
            // NOTE: pixels must be 16-byte aligned
            _mm_store_si128((__m128i* )&pixels[pixel_index], result_color);
        }
    }
}


void ConstructMandelbrot_Slow(PixelMatrix pixels, const ScreenParams* screen) {
    for (unsigned y_i = 0; y_i < screen->height; y_i++) {
        for (unsigned x_i = 0; x_i < screen->width; x_i++) {
            unsigned pixel_index = (y_i * screen->width + x_i) * 4;

            // x0 = (x_i - W/2) / zoom + x_center
            double x0 = ((double)x_i - (double)screen->width  / 2) * screen->scale + screen->center.x;
            double y0 = ((double)y_i - (double)screen->height / 2) * screen->scale + screen->center.y;

            double x = 0, y = 0, x2 = 0, y2 = 0;

            // FIXME: ставить итерацию =1 (Тогда ITERATIONS + 1) или =0 ?
            unsigned iteration = 0;
            while (x2 + y2 <= RADIUS && iteration < ITERATIONS) {
                y = 2 * x * y + y0;
                x = x2 - y2 + x0;

                x2 = x * x;
                y2 = y * y;

                iteration++;
            }

            uint8_t result_color = 255 * iteration / ITERATIONS;

            pixels[pixel_index + 0] = result_color; // RED
            pixels[pixel_index + 1] = result_color; // GREEN
            pixels[pixel_index + 2] = result_color; // BLUE
            pixels[pixel_index + 3] = 255;          // ALPHA
        }
    }
}
