#include "landsat.hpp"
#include "data.hpp"
#include "stats.hpp"

#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char **argv)
{
	if (argc < 3) {
		std::cerr << "usage: " << argv[0] << " [red-image] [near-infrared-image]" << std::endl;
		return ERR_ARG_COUNT;
	} else {
		const char *red_filename = argv[1];
		const char *nir_filename = argv[2];
		std::ifstream red_file(red_filename);
		std::ifstream nir_file(nir_filename);
		bool red_exists = red_file.good();
		bool nir_exists = nir_file.good();
		red_file.close();
		nir_file.close();
		if (!red_exists) {
			std::cerr << "could not open '" << red_filename << "'" << std::endl;
			return ERR_BAD_FILE;
		} else if (!nir_exists) {
			std::cerr << "could not open '" << nir_filename << "'" << std::endl;
			return ERR_BAD_FILE;
		} else {
			landsat::process_images(red_filename, nir_filename);
			return 0;
		}
	}
}

namespace landsat
{

	void process_images(const char *red, const char *near_infrared)
	{
		grid<pixel_t> *red_data = get_data(red);
		grid<pixel_t> *nir_data = get_data(near_infrared);
		array<regression_stats> *stats = get_all_window_regression_stats(*red_data, *nir_data);
		output_results(*stats);
		delete red_data;
		delete nir_data;
		delete stats;
	}

	void output_results(array<regression_stats> const &results)
	{
		std::cout << "window size, slope mean, slope stddev" << std::endl;
		for (size_t i = 0; i < results.size(); i++) {
			regression_stats reg = results[i];
			std::cout << reg.window_size << "," << reg.mean << "," << reg.stddev << std::endl;
		}
	}

	regression_stats *get_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size)
	{
		size_t window_count = (red.width() / size) * (red.height() / size);
		numeric_array slopes(window_count);
		numeric_t *ptr = slopes.data();
		for (size_t y = 0; y + size <= red.height() && y + size <= nir.height(); y += size) {
			for (size_t x = 0; x + size <= red.width() && x + size <= nir.width(); x += size) {
				grid<pixel_t> *red_sub = red.sub(x, y, size, size);
				grid<pixel_t> *nir_sub = nir.sub(x, y, size, size);
				linear_regression *reg = get_regression(red_sub, nir_sub);
				*ptr = reg->eq.slope;
				ptr++;
				delete reg;
				delete red_sub;
				delete nir_sub;
			}
		}
		// okay, we have slope data, now find statistics
		regression_stats *stats = new regression_stats;
		stats->mean = mean(slopes);
		stats->stddev = stddev(slopes);
		stats->window_size = size;
		return stats;
	}

	linear_regression *get_regression(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		numeric_array red_data(red.width() * red.height());
		numeric_array nir_data(nir.width() * nir.height());
		for (size_t y = 0; y < red.width(); y++) {
			for (size_t x = 0; x < red.height(); x++) {
				red_data[(y * red.height()) + x] = red.get(x, y);
				nir_data[(y * nir.height()) + y] = nir.get(x, y);
			}
		}
		return find_linear_regression(red_data, nir_data);
	}

	array<regression_stats> *get_all_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		size_t window_base = 2;
		size_t size_count = (size_t) log(red.width()) / log(window_base);
		array<regression_stats> *all_stats = new array<regression_stats>(size_count);
		size_t pos = 0;
		// we only do powers of two:
		for (size_t window_size = 1; window_size < red.height(); window_size *= window_base) {
			regression_stats *stats = get_window_regression_stats(red, nir, window_size);
			(*all_stats)[pos] = *stats;
			delete stats;
		}
		return all_stats;
	}

}
