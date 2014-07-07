// must come before including landsat.hpp
#define LOUDNESS_VAR landsat::get_loudness_level()

#include "analysis.hpp"
#include "landsat.hpp"

#include <cmath>

namespace landsat
{
	static window_regression_stats *get_window_regression_stats(
	 const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size,
	 bool force);
	static cell_regression_stats *get_cell_regression_stats(
	 grid<pixel_t> const &red, grid<pixel_t> const &nir, size_t size,
	 bool force);
	static window_regression_stats *get_hybrid_regression_stats(
	 const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size,
	 bool force, unsigned int base, int datapow);
	static bool is_good_data(const grid<pixel_t> &red,
	 const grid<pixel_t> &nir);
	static size_t size_pow(size_t base, size_t exp);	
	static size_t hybrid_subgroup_size(size_t group_size, unsigned int base,
	 int datapow);
	static int round(double num);

	static window_regression_stats *get_window_regression_stats(
	 const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size,
	 bool force)
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
		for (subr.y = 0; subr.y + size <= red.height() && subr.y +
		 size <= nir.height(); subr.y += size) {
			for (subr.x = 0; subr.x + size <= red.width() &&
			 subr.x + size <= nir.width(); subr.x += size) {
				const grid<pixel_t> red_sub(
				 const_cast<grid<pixel_t>*>(&red), subr);
				const grid<pixel_t> nir_sub(
				 const_cast<grid<pixel_t>*>(&nir), subr);
				if (force || is_good_data(red_sub, nir_sub)) {
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
				IF_NORMAL(std::cout << "\ranalyzing rows "
				 "starting at " << (subr.y / size) << "...");
				IF_NORMAL(std::cout.flush());
			}
		}
		IF_NORMAL(std::cout << std::endl);
		// okay, we have slope data, now find statistics
		window_regression_stats *stats = new window_regression_stats;
		// first, filter out the bad ones
		IF_VERBOSE(std::cout << "Found " << good_count);
		IF_VERBOSE(std::cout << " good sectors out of ");
		IF_VERBOSE(std::cout << window_count << std::endl);
		array<numeric_t> good_slope_data(good_count);
		array<numeric_t> good_intercept_data(good_count);
		array<numeric_t> good_r2_data(good_count);
		size_t good_data_cur = 0;
		for (size_t i = 0; i < window_count; i++) {
			if (goodness[i]) {
				good_slope_data[good_data_cur] = slopes[i];
				good_intercept_data[good_data_cur] =
				 intercepts[i];
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

	static cell_regression_stats *get_cell_regression_stats(
	 grid<pixel_t> const &red, grid<pixel_t> const &nir, size_t size,
	 bool force)
	{
					std::cout << "Red: " << red.size() << ", Nir: " << nir.size() << ", size: " << size << std::endl;
		size_t output_block = 100;
		rect<size_t> sub = {0, 0, size, size};
		size_t xCount = red.width() / size;
		size_t yCount = red.height() / size;
		IF_NORMAL(std::cout << yCount << " rows" << std::endl);
		size_t cell_count = xCount * yCount;
		array<numeric_t> red_avg(cell_count);
		array<numeric_t> nir_avg(cell_count);
		array<bool> goodness(cell_count);
		size_t good_count = 0;
		bool *goodness_ptr = goodness.data();
		numeric_t *red_avg_ptr = red_avg.data();
		numeric_t *nir_avg_ptr = nir_avg.data();
		for (sub.y = 0; sub.y + size <= red.height(); sub.y += size) {
			for (sub.x = 0; sub.x + size <= red.width();
			 sub.x += size) {
				const grid<pixel_t> sub_red(
				 const_cast<grid<pixel_t>*>(&red), sub);
				const grid<pixel_t> sub_nir(
				 const_cast<grid<pixel_t>*>(&nir), sub);
				if (force || is_good_data(sub_red, sub_nir)) {
					*red_avg_ptr = stats_mean(sub_red);
					*nir_avg_ptr = stats_mean(sub_nir);
					*goodness_ptr = true;
					good_count++;
				} else {
					IF_VERBOSE(std::cout << "Bad sector: ("
					 );
					IF_VERBOSE(std::cout << sub.x << ", ");
					IF_VERBOSE(std::cout << sub.y << ") ");
					*goodness_ptr = false;
				}
				red_avg_ptr++;
				nir_avg_ptr++;
				goodness_ptr++;
			}
			if ((sub.y / size) % output_block == 0) {
				IF_NORMAL(std::cout << "\ranalyzing rows ");
				IF_NORMAL(std::cout << "starting at ");
				IF_NORMAL(std::cout << (sub.y / size));
				IF_NORMAL(std::cout << "...");
				IF_NORMAL(std::cout.flush());
			}
		}
		IF_NORMAL(std::cout << std::endl);
		IF_VERBOSE(std::cout << "Found " << good_count << " good ");
		IF_VERBOSE(std::cout << "sectors out of " << cell_count);
		IF_VERBOSE(std::cout << std::endl);
		array<numeric_t> good_red_data(good_count);
		array<numeric_t> good_nir_data(good_count);
		size_t good_data_cur = 0;
		for (size_t i = 0; i < cell_count; i++) {
			if (goodness[i]) {
				good_red_data[good_data_cur] = red_avg[i];
				good_nir_data[good_data_cur] = nir_avg[i];
				good_data_cur++;
			}
		}
		linear_regression *reg = numeric_find_linear_regression(
		 good_red_data, good_nir_data);
		cell_regression_stats *stats = new cell_regression_stats;
		stats->cell_size = size;
		stats->regression = *reg;
		delete reg;
		return stats;
	}

	static window_regression_stats *get_hybrid_regression_stats(
	 const grid<pixel_t> &red, const grid<pixel_t> &nir, size_t size,
	 bool force, unsigned int base, int datapow)
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
		size_t subgroup_size = hybrid_subgroup_size(size, base,
		 datapow);
		rect<size_t> subr = {0, 0, size, size};
		for (subr.y = 0; subr.y + size <= red.height() && subr.y +
		 size <= nir.height(); subr.y += size) {
			for (subr.x = 0; subr.x + size <= red.width() &&
			 subr.x + size <= nir.width(); subr.x += size) {
				const grid<pixel_t> red_sub(
				 const_cast<grid<pixel_t>*>(&red), subr);
				const grid<pixel_t> nir_sub(
				 const_cast<grid<pixel_t>*>(&nir), subr);
				if (force || is_good_data(red_sub, nir_sub)) {
					cell_regression_stats *cell_stats =
					 get_cell_regression_stats(red_sub,
					 nir_sub, subgroup_size, true);
					linear_regression *reg =
					 &(cell_stats->regression);
					*slopes_ptr = reg->eq.slope;
					*intercepts_ptr = reg->eq.intercept;
					*r2s_ptr = reg->r2;
					*ptr_goodness = true;
					good_count++;
					delete cell_stats;
				} else {
					IF_VERBOSE(std::cout << "Bad sector: ");
					IF_VERBOSE(std::cout << "(" << subr.x);
					IF_VERBOSE(std::cout << ", " << subr.y);
					IF_VERBOSE(std::cout << ")<super> ");
					*ptr_goodness = false;
				}
				slopes_ptr++;
				intercepts_ptr++;
				r2s_ptr++;
				ptr_goodness++;
			}
			if ((subr.y / size) % output_block == 0) {
				IF_NORMAL(std::cout << "\ranalyzing rows "
				 "starting at " << (subr.y / size) << "...");
				IF_NORMAL(std::cout.flush());
			}
		}
		IF_NORMAL(std::cout << std::endl);
		// okay, we have slope data, now find statistics
		window_regression_stats *stats = new window_regression_stats;
		// first, filter out the bad ones
		IF_VERBOSE(std::cout << "Found " << good_count);
		IF_VERBOSE(std::cout << " good sectors out of ");
		IF_VERBOSE(std::cout << window_count << std::endl);
		array<numeric_t> good_slope_data(good_count);
		array<numeric_t> good_intercept_data(good_count);
		array<numeric_t> good_r2_data(good_count);
		size_t good_data_cur = 0;
		for (size_t i = 0; i < window_count; i++) {
			if (goodness[i]) {
				good_slope_data[good_data_cur] = slopes[i];
				good_intercept_data[good_data_cur] =
				 intercepts[i];
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

	static bool is_good_data(const grid<pixel_t> &red,
	 const grid<pixel_t> &nir)
	{
		bool has_nonzero = false;
		// at least one x must be different from the others
		bool diff_x = false;
		pixel_t initial = red.get(0, 0);
		for (size_t y = 0; y < red.height(); y++) {
			for (size_t x = 0; x < red.width(); x++) {
				if (!diff_x && red.get(x, y) != initial) {
					diff_x = true;
				}
				if (!has_nonzero && (red.get(x, y) != 0 ||
				 nir.get(x, y) != 0)) {
					has_nonzero = true;
				}
				if (diff_x && has_nonzero) {
					break;
				}
			}
		}
		return (has_nonzero && diff_x);
	}

	static size_t size_pow(size_t base, size_t exp)
	{
		size_t total = 0;
		if (exp == 0) {
			total = 1;
		} else {
			total = base;
			for (size_t i = 1; i < exp; i++) {
				total *= base;
			}
		}
		return total;
	}

	static size_t hybrid_subgroup_size(size_t group_size, unsigned int base,
	 int datapow)
	{
		size_t grp_pow = (size_t) round(log(group_size) /
		 log(base));
		size_t sub_pow = grp_pow - datapow;
		size_t subgroup_size = size_pow(base, sub_pow);
		return subgroup_size;
	}

	array<window_regression_stats> *analyze_windows(
	 const grid<pixel_t> &red, const grid<pixel_t> &nir, bool force,
	 unsigned int base, int startpow)
	{
		size_t stats_count = (size_t) ((log(red.width()) /
		 log(base)) - startpow);
		array<window_regression_stats> *all_stats =
		 new array<window_regression_stats>(stats_count);
		size_t pos = 0;
		// we only do powers of our size base
		for (size_t size = size_pow(base, startpow);
		 size * base < red.height(); size *= base) {
			IF_NORMAL(std::cout << "Checking window size ");
			IF_NORMAL(std::cout << size << "...");
			IF_NORMAL(std::cout << std::endl);
			window_regression_stats *stats =
			 get_window_regression_stats(red, nir, size, force);
			(*all_stats)[pos++] = *stats;
			delete stats;
		}
		return all_stats;
	}

	array<cell_regression_stats> *analyze_cells(const grid<pixel_t> &red,
	 const grid<pixel_t> &nir, bool force, unsigned int base, int startpow)
	{
		size_t stats_count = (size_t) ((log(red.width()) /
		 log(base)) - startpow);
		array<cell_regression_stats> *all_stats =
		 new array<cell_regression_stats>(stats_count);
		size_t pos = 0;
		for (size_t size = size_pow(base, startpow);
		 size * base < red.height(); size *= base) {
			IF_NORMAL(std::cout << "Checking cell size ");
			IF_NORMAL(std::cout << size << "..." << std::endl);
			cell_regression_stats *stats =
			 get_cell_regression_stats(red, nir, size, force);
			 (*all_stats)[pos++] = *stats;
			delete stats;
		}
		return all_stats;
	}

	array<window_regression_stats> *analyze_hybrid(const grid<pixel_t> &red,
	 const grid<pixel_t> &nir, bool force, unsigned int base, int startpow,
	 int datapow)
	{
		size_t stats_count = (size_t) ((log(red.width()) /
		 log(base)) - datapow);
		array<window_regression_stats> *all_stats =
		 new array<window_regression_stats>(stats_count);
		size_t pos = 0;
		// we only do powers of our size base:
		for (size_t size = size_pow(base, datapow);
		 size * base < red.height(); size *= base) {
			IF_NORMAL(std::cout << "Checking window size ");
			IF_NORMAL(std::cout << size << "...");
			IF_NORMAL(std::cout << std::endl);
			window_regression_stats *stats =
			 get_hybrid_regression_stats(red, nir, size, force,
			 base, datapow);
			(*all_stats)[pos++] = *stats;
			delete stats;
		}
		return all_stats;
	}

	static int round(double num)
	{
		bool neg = false;
		if (num < 0) {
			neg = true;
			num *= -1;
		}
		int rounded = static_cast<int>(num);
		double fpart = num - rounded;
		if (fpart >= 0.5) {
			rounded++;
		}
		if (neg) {
			rounded *= -1;
		}
		return rounded;
	}

}
