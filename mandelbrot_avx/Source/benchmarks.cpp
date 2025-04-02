#include "benchmarks.hpp"
#include "construct_mandelbrot.hpp"
#include "time.h"
#include "types.hpp"
#include "consts.hpp"
#include <vector>

typedef void (*TestFunctionType)(uint8_t* pixels, const ScreenParams* screen);

template<TestFunctionType TestFunction>
std::vector<clock_t> Benchmark_Run(const std::vector<ScreenParams>& test_screens){
	__declspec(align(32)) uint8_t pixels[1024 * 1024 * 4];
	
	std::vector<clock_t> benchmark_results;
	benchmark_results.reserve(test_screens.size());

	for (const ScreenParams& test_screen : test_screens) {
		clock_t sum_time = 0;

		for (unsigned iter = 0; iter < BENCHMARK_ITERATIONS; iter++) {
			clock_t start = clock();
			TestFunction(pixels, &test_screen);
			clock_t end = clock();

			sum_time += end - start;
		}

		benchmark_results.push_back(sum_time);
	}
	return benchmark_results;
}

void Benchmark_DumpToFile(const char* filename, const std::vector<clock_t>& benchmark_results) {
	FILE* fp;
	if (fopen_s(&fp, filename, "wb") != 0) {
		perror("Error: Benchmark_DumpToFile cannot OPEN file");
		exit(1);
	}

	fwrite(benchmark_results.data(), benchmark_results.size(), sizeof(clock_t), fp);

	if (fclose(fp) != 0) {
		perror("Error: Benchmark_DumpToFile cannot CLOSE file");
		exit(1);
	}
}

void RunAllBenchmarks() {
	std::vector<ScreenParams> test_screens;

	std::vector<clock_t> avx_results  = Benchmark_Run<ConstructMandelbrot_AVX2>(test_screens);
	std::vector<clock_t> slow_results = Benchmark_Run<ConstructMandelbrot_Slow>(test_screens);

	Benchmark_DumpToFile("benchmarks/avx.bin", avx_results);
	Benchmark_DumpToFile("benchmarks/slow.bin", slow_results);
}