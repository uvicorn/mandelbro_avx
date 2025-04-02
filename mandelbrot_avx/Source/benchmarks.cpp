#include "benchmarks.hpp"
#include "construct_mandelbrot.hpp"
#include "time.h"
#include "types.hpp"
#include "consts.hpp"
#include <vector>
#include <random>
#include <iostream>
#include "stdint.h"
#include <intrin.h>

#pragma intrinsic(__rdtsc)


typedef void (*TestFunctionType)(uint8_t* pixels, const ScreenParams* screen);

template<TestFunctionType TestFunction>
std::vector<uint64_t> Benchmark_Run(const std::vector<ScreenParams>& test_screens){
	uint8_t* pixels = (uint8_t* )malloc(2048 * 2048 * 4);

	std::vector<uint64_t> benchmark_results;
	benchmark_results.reserve(test_screens.size());

	for (const ScreenParams& test_screen : test_screens) {
		uint64_t sum_time = 0;

		for (unsigned iter = 0; iter < BENCHMARK_ITERATIONS; iter++) {
			//clock_t start = clock();
			uint64_t start = __rdtsc();
			TestFunction(pixels, &test_screen);
			uint64_t end = __rdtsc();
			//clock_t end = clock();

			sum_time += end - start;
		}

		std::cout << "SUM_TIME: " << sum_time << '\n';

		benchmark_results.push_back(sum_time);
	}
	return benchmark_results;
}

void Benchmark_DumpToFile(const char* filename, const std::vector<uint64_t>& benchmark_results) {
	FILE* fp;
	if (fopen_s(&fp, filename, "wb") != 0) {
		perror("Error: Benchmark_DumpToFile cannot OPEN file");
		exit(1);
	}

	fwrite(benchmark_results.data(), benchmark_results.size(), sizeof(uint64_t), fp);

	if (fclose(fp) != 0) {
		perror("Error: Benchmark_DumpToFile cannot CLOSE file");
		exit(1);
	}
}

std::vector<ScreenParams> GenerateTests() {
	std::vector<ScreenParams> test_screens;

	std::default_random_engine rnd{ std::random_device{}() };
	std::uniform_real_distribution<double> point_dist(-1.0, 1.0);
	std::uniform_int_distribution<unsigned> width_height_dist(50u, 2048u/4);
	std::uniform_int_distribution<unsigned> scale_power_dist(1, 40);

	for (unsigned test_index = 1; test_index < TESTS_COUNT + 1; test_index++) {
		ScreenParams result_screen;
		
		result_screen.center.x = point_dist(rnd);
		result_screen.center.y = point_dist(rnd);

		result_screen.height = width_height_dist(rnd) * 4;
		result_screen.width = width_height_dist(rnd) * 4;
		result_screen.scale = powf(0.5, test_index);

		test_screens.push_back(result_screen);
	}

	return test_screens;
}

void RunAllBenchmarks() {
	std::vector<ScreenParams> test_screens = GenerateTests();

	std::vector<uint64_t> avx_results  = Benchmark_Run<ConstructMandelbrot_AVX2>(test_screens);
	std::vector<uint64_t> slow_results = Benchmark_Run<ConstructMandelbrot_Slow>(test_screens);

	Benchmark_DumpToFile("benchmarks/avx.bin", avx_results);
	Benchmark_DumpToFile("benchmarks/slow.bin", slow_results);
}