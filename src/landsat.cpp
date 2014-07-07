#define LOUDNESS_VAR landsat::loudness_level

#include "landsat.hpp"
#include "analysis.hpp"
#include "parse.hpp"
#include "stats.hpp"
#include "data.hpp"

#include <iostream>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <string>
#include <sstream>

int main(int argc, char **argv);

namespace landsat
{
	static int loudness_level = LOUDNESS_NORMAL;
	static void print_help(const char *progname);
	static void print_version();
	void print_grid(grid<pixel_t> const &g);
	void print_rect(rect<size_t> const &r);
	static const char *usage(const char *progname);
	static rect<size_t> *interpret_window(rect<int> const &cli_window,
	 grid<pixel_t> const &data);
	
	// Returns zero if successful.
	//
	// If the set value ended up having to get defaulted,
	// returns less than zero if set to the minimum size and
	// returns greater than zero if set to the maximum size.
	//
	// Makes no assumtions about cli_coord's size.
	static int interpret_window_dimension(size_t &set, int cli_length,
	 int cli_coord, size_t data_length);
	static void interpret_window_coordinate(size_t &set, int cli);
	static void print_offset_warning(const char *dimension,
	 const char *comp_op, const char *comp);
	static void process_images(const char *red, const char *near_infrared,
	 rect<int> &cli_window, int mode);
	static void output_window_results(
	 const array<window_regression_stats> &results);
	static void output_cell_results(
	 const array<cell_regression_stats> &results);
}

int main(int argc, char **argv)
{
	landsat::cli_arguments *args = new landsat::cli_arguments;
	int status = parse_args(argc, argv, args);
	if (status == 0) {
		switch (args->mode) {
			case MODE_NORMAL:
			case MODE_CELLS:
			case MODE_HYBRID:
				landsat::loudness_level = args->loudness;
				landsat::process_images(args->red_filename,
				 args->nir_filename, args->window, args->mode);
				break;

			case MODE_HELP:
				landsat::print_help(argv[0]);
				break;

			case MODE_VERSION:
				landsat::print_version();
				break;
		}
	} else if (status == ERR_MISSING_ARG || status == ERR_MISSING_OPT_ARG ||
	 status == ERR_UNKNOWN_OPT) {
		std::cerr << "usage: " << landsat::usage(argv[0]) << std::endl;
	}
	delete args;
	return status;
}

namespace landsat
{

	int get_loudness_level()
	{
		return loudness_level;
	}

	static void print_help(const char *progname)
	{
		std::cout << usage(progname) << "\n";
		std::cout << "\n";
		std::cout << "Pass two filenames containing TIFF data from the "
				" the LANDSAT-7 satellite. The\n";
		std::cout << "first must containing red light data and the "
				"second must contain near-infrared\n";
		std::cout << "light data. The correlation data will then be "
				"printed to stdout.\n";
		std::cout << "\n";
		std::cout << "Available options:\n";
		std::cout << "-h | --help                       -   show this "
				"help\n";
		std::cout << "-i | --version                    -   show "
				"version information\n";
		std::cout << "-q | --quiet, --silent            -   supress "
				"all output except for data\n";
		std::cout << "-v | --verbose                    -   display "
				"additional information\n";
		std::cout << "-p [x],[y] | --position=[x],[y]   -   start "
				"position of analysis window\n";
		std::cout << "-s [w]x[h] | --size=[w]x[h]       -   size of "
				"analysis window\n";
		std::cout << "-c | --cells                      -   analyze "
				"based on cell average values\n";
		std::cout << "-f | --fixed                      -   analyze "
				"with fixed number of data points\n";
		std::cout << "\n";
		std::cout << "Report bugs to: " PACKAGE_BUGREPORT "\n";
		std::cout << PACKAGE_NAME " home page: " PACKAGE_URL "\n";
		std::cout << "For additional information, see the man page\n";
		std::cout << std::endl;
	}

	static void print_version()
	{
		std::cout << "Landsat Data Correlation Tool " VERSION;
		std::cout << std::endl;
	}

	static const char *usage(const char *progname)
	{
		static char *buffer = NULL;
		if (buffer == NULL) {
			const char *fmt = "%s <options> [red-image] "
					"[near-infrared-image]";
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
		std::cout << "{ x=" << r.x << ", y=" << r.y << ", width=";
		std::cout << r.width << ", height=" << r.height << " }";
		std::cout << std::endl;
	}
	
	static void process_images(const char *red, const char *near_infrared,
	 rect<int> &cli_window, int mode)
	{
		IF_VERBOSE(std::cout << "loading red image..." << std::endl);
		grid<pixel_t> *red_data = get_data(red);
		IF_VERBOSE(std::cout << "loading near-infrared image...");
		IF_VERBOSE(std::cout << std::endl);
		grid<pixel_t> *nir_data = get_data(near_infrared);
		rect<size_t> *data_window = interpret_window(cli_window,
		 *red_data);
		grid<pixel_t> *sub_red = new grid<pixel_t>(red_data,
		 *data_window);
		grid<pixel_t> *sub_nir = new grid<pixel_t>(nir_data,
		 *data_window);
		delete data_window;
		if (mode == MODE_NORMAL) {
			array<window_regression_stats> *stats = analyze_windows(
			 *sub_red, *sub_nir);
			output_window_results(*stats);
			delete stats;
		} else if (mode == MODE_CELLS) {
			array<cell_regression_stats> *stats = analyze_cells(
			 *sub_red, *sub_nir);
			output_cell_results(*stats);
			delete stats;
		} else if (mode == MODE_HYBRID) {
			array<window_regression_stats> *stats = analyze_hybrid(
			 *sub_red, *sub_nir);
			output_window_results(*stats);
			delete stats;
		} else {
			// should never happen
			std::cerr << "Error: invalid program mode" << std::endl;
		}
		delete sub_red;
		delete sub_nir;
		delete red_data;
		delete nir_data;
	}

	static int interpret_window_dimension(size_t &set, int cli_length,
	 int cli_coord, size_t data_length)
	{
		int working_set;
		int error = 0;
		if (cli_length == 0) {
			working_set = data_length;
		} else if (cli_length < 0) {
			// without cast, cli_length would be promoted to size_t
			working_set = static_cast<signed int>(data_length) +
			 cli_length;
		} else {
			working_set = cli_length;
		}
		if (cli_coord < 0) {
			// underflow check
			if (working_set >= 0 || working_set + cli_coord < 0) {
				working_set += cli_coord;
			} else {
				// this is okay because we're about to check for
				// low vals
				working_set = 0;
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
			if (sizeof(size_t) >= sizeof(unsigned int) ||
			 working_set <= imaxval) {
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

	static void interpret_window_coordinate(size_t &set, int cli)
	{
		set = (cli < 0) ? 0 : cli;
	}

	static void print_offset_warning(const char *dimension,
	 const char *comp_op, const char *comp)
	{
		std::cerr << "Warning: given offsets result in analysis "
				"window ";
		std::cerr << dimension << " " << comp_op << " than " << comp;
		std::cerr << ".\n";
		std::cerr << "Using " << comp << " for analysis window ";
		std::cerr << dimension << " instead.\n";
		std::cerr << std::endl;
	}

	static rect<size_t> *interpret_window(rect<int> const &window,
	 grid<pixel_t> const &data)
	{
		rect<size_t> *interpreted = new rect<size_t>;
		// data.width() / .height() are bound by the size needed to hold
		// pixel_t.
		// As long as pixel_t is def'd to something smaller than int,
		// we can assume data dimensions will fit in a signed int.
		int err_w = interpret_window_dimension(interpreted->width,
		 window.width, window.x, data.width());
		int err_h = interpret_window_dimension(interpreted->height,
		 window.height, window.y, data.height());
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
			print_offset_warning("height", "greater",
			 "image height");
		}
		return interpreted;
	}

	static void output_window_results(
	 array<window_regression_stats> const &results)
	{
		IF_NORMAL(std::cout << "(y = mx + b)" << std::endl);
		IF_NORMAL(std::cout << "size, mean m, stdev m, mean b, ");
		IF_NORMAL(std::cout << "stdev b, mean r2, stdev r2");
		IF_NORMAL(std::cout << std::endl);
		for (size_t i = 0; i < results.size(); i++) {
			window_regression_stats reg = results[i];
			IF_QUIET(std::cout << reg.window_size << ' ');
			IF_QUIET(std::cout << reg.slope_mean << ' ');
			IF_QUIET(std::cout << reg.slope_stddev << ' ');
			IF_QUIET(std::cout << reg.intercept_mean << ' ');
			IF_QUIET(std::cout << reg.intercept_stddev << ' ');
			IF_QUIET(std::cout << reg.r2_mean << ' ');
			IF_QUIET(std::cout << reg.r2_stddev << std::endl);
		}
	}

	static void output_cell_results(
	 array<cell_regression_stats> const &results)
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

}
