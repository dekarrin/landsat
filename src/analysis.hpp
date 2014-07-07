#include "defs.hpp"
#include "stats.hpp"

namespace landsat
{
	struct window_regression_stats
	{
		numeric_t slope_mean;
		numeric_t slope_stddev;
		numeric_t intercept_mean;
		numeric_t intercept_stddev;
		numeric_t r2_mean;
		numeric_t r2_stddev;
		size_t window_size;
	};

	struct cell_regression_stats
	{
		linear_regression regression;
		size_t cell_size;
	};
	
	// For each cell size, average is taken of each group of that size and
	// used as the plotted data point.
	//
	// Returns an array of stats that contain both the size and the linear
	// regression found for that size.
	array<cell_regression_stats> *analyze_cells(grid<pixel_t> const &red,
	 grid<pixel_t> const &nir, bool force, unsigned int base, int startpow);

	// For each window size, a correlation is found in each group of that
	// size using each pixel as the plotted data point.
	//
	// Returns an array of stats that contain the size of the window as well
	// as a series of stats consisting of the mean and standard deviation
	// of the slope, intercept, and coefficient of correlation for the set
	// of regressions found for each of the sizes.
	array<window_regression_stats> *analyze_windows(
	 grid<pixel_t> const &red, grid<pixel_t> const &nir, bool force,
	 unsigned int base, int startpow);

	// Combines the processes used by analyze_windows and analyze_cells.
	// Similar process as analyze_windows, but uses the same number of data
	// points for each regression. This is accomplished by splitting each
	// group into the same number of subgroups and using the average of each
	// subgroup as the plotted data point.
	//
	// Every size will use the same number of data points as the minimum
	// size. The minimum size is implementation-defined.
	//
	// Returns the same type of data structure as analyze_windows.
	array<window_regression_stats> *analyze_hybrid(grid<pixel_t> const &red,
	 grid<pixel_t> const &nir, bool force, unsigned int base, int startpow,
	 int datapow);

}
