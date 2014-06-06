#include "landsat.hpp"
#include "parse.hpp"

#include <iostream>
#include <cstdio>
#include <cmath>
#include <cstring>
#include <iomanip>
#include <string>
#include <sstream>

#define IS_LOUDNESS(x) landsat::loudness_level >= x
#define IS_QUIET IS_LOUDNESS(LOUDNESS_QUIET)
#define IS_NORMAL IS_LOUDNESS(LOUDNESS_NORMAL)
#define IS_VERBOSE IS_LOUDNESS(LOUDNESS_VERBOSE)

#define IF_LOUDNESS(x, y) do { if(landsat::loudness_level >= x) {y;} } while(0)
#define IF_QUIET(x) IF_LOUDNESS(LOUDNESS_QUIET, x)
#define IF_NORMAL(x) IF_LOUDNESS(LOUDNESS_NORMAL, x) 
#define IF_VERBOSE(x) IF_LOUDNESS(LOUDNESS_VERBOSE, x)

#include "cells.cpp"

int main(int argc, char **argv)
{
	landsat::cli_arguments *args = new landsat::cli_arguments;
	int status = parse_args(argc, argv, args);
	if (status == 0) {
		switch (args->mode) {
			case MODE_NORMAL:
			case MODE_CELLS:
				landsat::loudness_level = args->loudness;
				landsat::process_images(args->red_filename, args->nir_filename, args->window, args->mode);
				break;

			case MODE_HELP:
				landsat::print_help(argv[0]);
				break;

			case MODE_VERSION:
				landsat::print_version();
				break;
		}
	} else if (status == ERR_MISSING_ARG || status == ERR_MISSING_OPT_ARG || status == ERR_UNKNOWN_OPT) {
		std::cerr << "usage: " << landsat::usage(argv[0]) << std::endl;
	}
	delete args;
	return status;
}

namespace landsat
{

	void print_help(const char *progname)
	{
		std::cout << usage(progname) << "\n";
		std::cout << "\n";
		std::cout << "Pass two filenames containing TIFF data from the LANDSAT-7 satellite. The\n";
		std::cout << "first must containing red light data and the second must contain\n";
		std::cout << "near-infrared light data. The data correlation data will then be printed\n";
		std::cout << "to stdout.\n";
		std::cout << "\n";
		std::cout << "Available options:\n";
		std::cout << "-h | --help                       -   show this help\n";
		std::cout << "-i | --version                    -   show version information\n";
		std::cout << "-q | --quiet, --silent            -   supress all output except for data\n";
		std::cout << "-v | --verbose                    -   display additional information\n";
		std::cout << "-p [x],[y] | --position=[x],[y]   -   start position of analysis window\n";
		std::cout << "-s [w]x[h] | --size=[w]x[h]       -   size of analysis window\n";
		std::cout << "-c | --cells                      -   analyze based on cell average values\n";
		std::cout << "\n";
		std::cout << "Report bugs to: " PACKAGE_BUGREPORT "\n";
		std::cout << PACKAGE_NAME " home page: " PACKAGE_URL "\n";
		std::cout << "For additional information, see the man page\n";
		std::cout << std::endl;
	}

	void print_version()
	{
		std::cout << "Landsat Data Correlation Tool " VERSION << std::endl;
	}

	const char *usage(const char *progname)
	{
		static char *buffer = NULL;
		if (buffer == NULL) {
			const char *fmt = "%s <options> [red-image] [near-infrared-image]";
			size_t proglen = strlen(progname);
			size_t fmtlen = strlen(fmt) - 2;
			buffer = new char[(proglen + fmtlen) - 1];
			sprintf(buffer, fmt, progname);
		}
		return buffer;
	}

	void print_grid(grid<pixel_t> const &grid)
	{
		std::cout << std::setw(8) << std::left;
		for (size_t y = 0; y < grid.height(); y++) {
			for (size_t x = 0; x < grid.width(); x++) {
				if (x + 1 < grid.width()) {
					std::cout << std::setw(8);
				}
				std::ostringstream os;
				os << "0x" << std::hex << grid.get(x, y);
				std::cout << std::left << os.str();
			}
			std::cout << std::endl;
		}
	}

	void print_rect(rect<size_t> const &r)
	{
		std::cout << "{ x=" << r.x << ", y=" << r.y << ", width=" << r.width << ", height=" << r.height << " }" << std::endl;
	}
	
	void process_images(const char *red, const char *near_infrared,
	 rect<int> &cli_window, int mode)
	{
		IF_VERBOSE(std::cout << "loading red image..." << std::endl);
		grid<pixel_t> *red_data = get_data(red);
		IF_VERBOSE(std::cout << "loading near-infrared image..." << std::endl);
		grid<pixel_t> *nir_data = get_data(near_infrared);
		rect<size_t> *data_window = interpret_window(cli_window, *red_data);
		grid<pixel_t> *sub_red = new grid<pixel_t>(red_data, *data_window);
		grid<pixel_t> *sub_nir = new grid<pixel_t>(nir_data, *data_window);
		delete data_window;
		if (mode == MODE_NORMAL) {
			array<regression_stats> *stats = get_all_window_regression_stats(*sub_red, *sub_nir);
			output_window_results(*stats);
			delete stats;
		} else if (mode == MODE_CELLS) {
			array<cell_regression_stats> *stats =
			 get_all_cell_regression_stats(*sub_red, *sub_nir);
			output_cell_results(*stats);
			delete stats;
		} else {
			// should never happen
		}
		delete sub_red;
		delete sub_nir;
		delete red_data;
		delete nir_data;
	}

	int interpret_window_dimension(size_t &set, int cli_length, int cli_coord, size_t data_length)
	{
		int working_set;
		int error = 0;
		if (cli_length == 0) {
			working_set = data_length;
		} else if (cli_length < 0) {
			// without cast, cli_length would be 'promoted' to size_t
			working_set = static_cast<signed int>(data_length) + cli_length;
		} else {
			working_set = cli_length;
		}
		if (cli_coord < 0) {
			// underflow check
			if (working_set >= 0 || working_set + cli_coord < 0) {
				working_set += cli_coord;
			} else {
				working_set = 0; // this is okay because we're about to check for low vals
			}
		}
		size_t actual_coord;
		interpret_window_coordinate(actual_coord, cli_coord);
		if (working_set < 1) {
			error = -1;
			set = 1;
		} else {
			size_t maxval = static_cast<size_t>(-1);
			unsigned int imaxval = static_cast<signed int>(maxval);
			if (sizeof(size_t) >= sizeof(unsigned int) || working_set <= imaxval) {
				set = working_set;
			} else {
				set = maxval;
			}
			size_t bound = set + actual_coord;
			bound = (bound > 0) ? bound : maxval;
			if (bound > data_length) {
				error = 1;
				set = data_length - actual_coord;
			}
		}
		return error;
	}

	void interpret_window_coordinate(size_t &set, int cli)
	{
		set = (cli < 0) ? 0 : cli;
	}

	void print_offset_warning(const char *dimension, const char *comp_op, const char *comp)
	{
		std::cerr << "Warning: given offsets result in analysis window " << dimension << " ";
		std::cerr << comp_op << " than " << comp << ".\n";
		std::cerr << "Using " << comp << " for analysis window " << dimension << " instead.\n";
		std::cerr << std::endl;
	}

	rect<size_t> *interpret_window(rect<int> const &window,
	 grid<pixel_t> const &data)
	{
		rect<size_t> *interpreted = new rect<size_t>;
		// data.width() / .height() are bound by the size needed to hold pixel_t
		// As long as pixel_t is def'd to something smaller than int,
		// we can assume data dimensions will fit in a signed int.
		int err_w = interpret_window_dimension(interpreted->width, window.width, window.x, data.width());
		int err_h = interpret_window_dimension(interpreted->height, window.height, window.y, data.height());
		interpret_window_coordinate(interpreted->x, window.x);
		interpret_window_coordinate(interpreted->y, window.y);
		if (err_w < 0 && IS_NORMAL) {
			print_offset_warning("width", "less", "1");
		} else if (err_w > 0 && IS_NORMAL) {
			print_offset_warning("width", "greater", "image width");
		}
		if (err_h < 0 && IS_NORMAL) {
			print_offset_warning("height", "less", "1");
		} else if (err_h > 0 && IS_NORMAL) {
			print_offset_warning("height", "greater", "image height");
		}
		return interpreted;
	}

	void output_window_results(array<regression_stats> const &results)
	{
		IF_NORMAL(std::cout << "(y = mx + b)" << std::endl);
		IF_NORMAL(std::cout << "size, mean m, stdev m, mean b, ");
		IF_NORMAL(std::cout << "stdev b, mean r2, stdev r2");
		IF_NORMAL(std::cout << std::endl);
		for (size_t i = 0; i < results.size(); i++) {
			regression_stats reg = results[i];
			IF_QUIET(std::cout << reg.window_size << ' ');
			IF_QUIET(std::cout << reg.slope_mean << ' ');
			IF_QUIET(std::cout << reg.slope_stddev << ' ');
			IF_QUIET(std::cout << reg.intercept_mean << ' ');
			IF_QUIET(std::cout << reg.intercept_stddev << ' ');
			IF_QUIET(std::cout << reg.r2_mean << ' ');
			IF_QUIET(std::cout << reg.r2_stddev << std::endl);
		}
	}

	void output_cell_results(array<cell_regression_stats> const &results)
	{
		IF_NORMAL(std::cout << "window size, slope, intercept, r2");
		IF_NORMAL(std::cout << std::endl);
		for (size_t i = 0; i < results.size(); i++) {
			cell_regression_stats stats = results[i];
			linear_regression reg = stats.regression;
			IF_QUIET(std::cout << stats.cell_size << ' ');
			IF_QUIET(std::cout << reg.eq.slope << ' ');
			IF_QUIET(std::cout << reg.eq.intercept << ' ');
			IF_QUIET(std::cout << reg.r2 << ' ' << std::endl);
		}
	}

	regression_stats *get_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size)
	{
		size_t output_block = 100;
		size_t row_count = (red.height() / size);
		IF_NORMAL(std::cout << row_count << " rows" << std::endl);
		size_t window_count = (red.width() / size) * row_count;
		array<numeric_t> slopes(window_count);
		array<numeric_t> intercepts(window_count);
		array<numeric_t> r2s(window_count);
		array<bool> goodness(window_count);
		numeric_t *slopes_ptr = slopes.data();
		numeric_t *intercepts_ptr = intercepts.data();
		numeric_t *r2s_ptr = r2s.data();
		bool *ptr_goodness = goodness.data();
		size_t good_count = 0;
		rect<size_t> subr = {0, 0, size, size};
		for (subr.y = 0; subr.y + size <= red.height() && subr.y + size <= nir.height(); subr.y += size) {
			for (subr.x = 0; subr.x + size <= red.width() && subr.x + size <= nir.width(); subr.x += size) {
				const grid<pixel_t> red_sub(
				 const_cast<grid<pixel_t>*>(&red), subr);
				const grid<pixel_t> nir_sub(
				 const_cast<grid<pixel_t>*>(&nir), subr);
				if (is_good_data(red_sub, nir_sub)) {
					linear_regression *reg =
					 stats_find_linear_regression(red_sub,
					 nir_sub);
					*slopes_ptr = reg->eq.slope;
					*intercepts_ptr = reg->eq.intercept;
					*r2s_ptr = reg->r2;
					*ptr_goodness = true;
					good_count++;
					delete reg;
				} else {
					IF_VERBOSE(std::cout << "Bad sector: ");
					IF_VERBOSE(std::cout << "(" << subr.x);
					IF_VERBOSE(std::cout << ", " << subr.y);
					IF_VERBOSE(std::cout << ") ");
					*ptr_goodness = false;
				}
				slopes_ptr++;
				intercepts_ptr++;
				r2s_ptr++;
				ptr_goodness++;
			}
			if ((subr.y / size) % output_block == 0) {
				IF_NORMAL(std::cout << "\ranalyzing rows starting at " << (subr.y / size) << "...");
				IF_NORMAL(std::cout.flush());
			}
		}
		IF_NORMAL(std::cout << std::endl);
		// okay, we have slope data, now find statistics
		regression_stats *stats = new regression_stats;
		// first, filter out the bad ones
		IF_VERBOSE(std::cout << "Found " << good_count << " good sectors out of " << window_count << std::endl);
		array<numeric_t> good_slope_data(good_count);
		array<numeric_t> good_intercept_data(good_count);
		array<numeric_t> good_r2_data(good_count);
		size_t good_data_cur = 0;
		for (size_t i = 0; i < window_count; i++) {
			if (goodness[i]) {
				good_slope_data[good_data_cur] = slopes[i];
				good_intercept_data[good_data_cur] = intercepts[i];
				good_r2_data[good_data_cur] = r2s[i];
				good_data_cur++;
			}
		}
		stats->slope_mean = numeric_mean(good_slope_data);
		stats->slope_stddev = numeric_stddev(good_slope_data);
		stats->intercept_mean = numeric_mean(good_intercept_data);
		stats->intercept_stddev = numeric_stddev(good_intercept_data);
		stats->r2_mean = numeric_mean(good_r2_data);
		stats->r2_stddev = numeric_stddev(good_r2_data);
		stats->window_size = size;
		return stats;
	}

	bool is_good_data(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		bool has_nonzero = false;
		bool diff_x = false; // at least one x MUST be different from the others
		pixel_t initial = red.get(0, 0);
		for (size_t y = 0; y < red.height(); y++) {
			for (size_t x = 0; x < red.width(); x++) {
				if (!diff_x && red.get(x, y) != initial) {
					diff_x = true;
				}
				if (!has_nonzero && (red.get(x, y) != 0 || nir.get(x, y) != 0)) {
					has_nonzero = true;
				}
				if (diff_x && has_nonzero) {
					break;
				}
			}
		}
		return (has_nonzero && diff_x);
	}

	array<regression_stats> *get_all_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		size_t window_base = 2;
		size_t size_count = (size_t) ((log(red.width()) /
		 log(window_base)) - 1);
		array<regression_stats> *all_stats =
		 new array<regression_stats>(size_count);
		size_t pos = 0;
		// we only do powers of two:
		for (size_t window_size = window_base;
		 window_size * window_base < red.height();
		 window_size *= window_base) {
			IF_NORMAL(std::cout << "Checking window size " << window_size << "..." << std::endl);
			regression_stats *stats = get_window_regression_stats(red, nir, window_size);
			(*all_stats)[pos++] = *stats;
			delete stats;
		}
		return all_stats;
	}

}
