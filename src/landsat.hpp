#pragma once

#include "defs.hpp"
#include "stats.hpp"
#include "data.hpp"

#include <iostream>

int main(int argc, char **argv);

namespace landsat
{
	static int loudness_level = LOUDNESS_NORMAL;

	void print_help(const char *progname);
	void print_version();
	void print_grid(grid<pixel_t> const &g);
	void print_rect(rect<size_t> const &r);
	const char *usage(const char *progname);
	rect<size_t> *interpret_window(rect<int> const &cli_window,
	 grid<pixel_t> const &data);
	
	// Returns zero if successful.
	//
	// If the set value ended up having to get defaulted,
	// returns less than zero if set to the minimum size and
	// returns greater than zero if set to the maximum size.
	//
	// Makes no assumtions about cli_coord's size.
	int interpret_window_dimension(size_t &set, int cli_length,
	 int cli_coord, size_t data_length);
	void interpret_window_coordinate(size_t &set, int cli);
	void print_offset_warning(const char *dimension, const char *comp_op,
	 const char *comp);
	void process_images(const char *red, const char *near_infrared,
	 rect<int> &cli_window, int mode);
	void output_window_results(const array<regression_stats> &results);
	void output_cell_results(const array<cell_regression_stats> &results);
}
