#include "landsat.hpp"
#include "data.hpp"
#include "stats.hpp"

#include <iostream>
#include <fstream>
#include <cmath>

int main(int argc, char **argv)
{
	std::cout << VERSION << std::endl;
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
	bool is_good_data(grid<pixel_t> const &r, grid<pixel_t> const &n);

	void process_images(const char *red, const char *near_infrared)
	{
		grid<pixel_t> *red_data = get_data(red);
		grid<pixel_t> *nir_data = get_data(near_infrared);
		grid<pixel_t> *sub_red = new grid<pixel_t>(red_data, 3000, 3000, 600, 600);
		grid<pixel_t> *sub_nir = new grid<pixel_t>(nir_data, 3000, 3000, 600, 600);
		array<regression_stats> *stats = get_all_window_regression_stats(*sub_red, *sub_nir);
		output_results(*stats);
		delete sub_red;
		//delete sub_nir;
		delete red_data;
		//delete nir_data;
		//delete stats;
	}

	void output_results(array<regression_stats> const &results)
	{
		std::cout << "window size, slope mean, slope stddev " << results.size() << std::endl;
		for (size_t i = 0; i < results.size(); i++) {
			regression_stats reg = results[i];
			std::cout << reg.window_size << "," << reg.mean << "," << reg.stddev << std::endl;
		}
	}
}
#include <cmath>
namespace landsat {

	regression_stats *get_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size)
	{
		size_t output_block = 100;
		size_t row_count = (red.height / size);
		std::cout << row_count << " rows" << std::endl;
		size_t window_count = (red.width / size) * row_count;
		numeric_array slopes(window_count);
		array<bool> slopes_goodness(window_count);
		numeric_t *ptr = slopes.data();
		bool *ptr_goodness = slopes_goodness.data();
		size_t good_count = 0;
		for (size_t y = 0; y + size <= red.height && y + size <= nir.height; y += size) {
			for (size_t x = 0; x + size <= red.width && x + size <= nir.width; x += size) {
				grid<pixel_t> const *red_sub = new grid<pixel_t>(const_cast<grid<pixel_t>*>(&red), x, y, size, size);
				grid<pixel_t> const *nir_sub = new grid<pixel_t>(const_cast<grid<pixel_t>*>(&nir), x, y, size, size);
				if (is_good_data(*red_sub, *nir_sub)) {
					linear_regression *reg = get_regression(*red_sub, *nir_sub);
					*ptr = reg->eq.slope;
					*ptr_goodness = true;
					delete reg;
				} else {
					*ptr_goodness = false;
				}
				good_count++;
				ptr++;
				ptr_goodness++;
				delete red_sub;
				delete nir_sub;
			}
			if (y % output_block == 0) {
				std::cout << "\ranalyzing rows starting at " << y << "...";
				std::cout.flush();
			}
		}
		std::cout << std::endl;
		// okay, we have slope data, now find statistics
		regression_stats *stats = new regression_stats;
		// first, filter out the bad ones
		numeric_array good_data(good_count);
		numeric_t *good_data_ptr = good_data.data();
		for (size_t i = 0; i < window_count; i++) {
			if (slopes_goodness[i]) {
				*good_data_ptr = slopes[i];
				good_data_ptr++;
			}
		}
		stats->mean = mean(good_data);
		stats->stddev = stddev(good_data);
		stats->window_size = size;
		return stats;
	}

	bool is_good_data(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		bool has_nonzero = false;
		for (size_t y = 0; y < red.height; y++) {
			for (size_t x = 0; x < red.height; x++) {
				if (red.data[(y * red.width) + x] != 0 || nir.data[(y * nir.width) + x] != 0) {
					has_nonzero = true;
					break;
				}
			}
		}
		return has_nonzero;
	}

	linear_regression *get_regression(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		numeric_array red_data(red.width * red.height);
		numeric_array nir_data(nir.width * nir.height);
		for (size_t y = 0; y < red.width; y++) {
			for (size_t x = 0; x < red.height; x++) {
				red_data[(y * red.width) + x] = red.data[(y * red.width) + x];
				nir_data[(y * nir.width) + x] = nir.data[(y * nir.width) + x];
			}
		}
		return find_linear_regression(red_data, nir_data);
	}

	array<regression_stats> *get_all_window_regression_stats(const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		size_t window_base = 2;
		size_t size_count = (size_t) (log(red.width) / log(window_base));
		array<regression_stats> *all_stats = new array<regression_stats>(size_count);
		size_t pos = 0;
		// we only do powers of two:
		for (size_t window_size = 1; window_size < red.height; window_size *= window_base) {
			std::cout << "Checking window size " << window_size << "..." << std::endl;
			regression_stats *stats = get_window_regression_stats(red, nir, window_size);
			(*all_stats)[pos++] = *stats;
			delete stats;
		}
		return all_stats;
	}

}
