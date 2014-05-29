#pragma once

#include "defs.hpp"
#include "stats.hpp"
#include "data.hpp"

#include <iostream>

#define IF_LOUDNESS_CHECK(x, y) do { if(landsat::loudness_level >= x) {y;} } while(0)
#define IF_QUIET(x) IF_LOUDNESS_CHECK(LOUDNESS_QUIET, x)
#define IF_NORMAL(x) IF_LOUDNESS_CHECK(LOUDNESS_NORMAL, x) 
#define IF_VERBOSE(x) IF_LOUDNESS_CHECK(LOUDNESS_VERBOSE, x)

int main(int argc, char **argv);

namespace landsat
{
	static int loudness_level = LOUDNESS_NORMAL;

	struct regression_stats
	{
		numeric_t mean;
		numeric_t stddev;
		size_t window_size;
	};

	void print_help(const char *progname);
	void print_version();
	const char *usage(const char *progname);
	void translate_window(rect &window, grid<pixel_t> const &data);
	bool is_good_data(grid<pixel_t> const &red, grid<pixel_t> const &nir);
	void process_images(const char *red, const char *near_infrared, rect &window);
	void output_results(const array<regression_stats> &results);
	regression_stats *get_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size);
	linear_regression *get_regression(const grid<pixel_t> &red, const grid<pixel_t> &nir);
	array<regression_stats> *get_all_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir);
}
