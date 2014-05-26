#pragma once

#include "types.hpp"

#define ERR_ARG_COUNT 1
#define ERR_BAD_FILE 2

int main(int argc, char **argv);

namespace landsat
{
	struct regression_stats
	{
		numeric_t mean;
		numeric_t stddev;
		size_t window_size;
	};

	void process_images(const char *red, const char *near_infrared);
	void output_results(const array<regression_stats> &results);
	regression_stats *get_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size);
	linear_regression *get_regression(const grid<pixel_t> &red, const grid<pixel_t> &nir);
	array<regression_stats> *get_all_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir);
}
