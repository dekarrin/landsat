#pragma once

#include "defs.hpp"
#include <cstddef>

namespace landsat
{
	typedef double numeric_t;
	typedef pixel_t stats_t;

	struct linear_eq
	{
		numeric_t intercept;
		numeric_t slope;
	};

	struct linear_regression
	{
		linear_eq eq;
		numeric_t r2;
	};

	stats_t stats_min(sequence<stats_t> const &data);
	numeric_t numeric_min(sequence<numeric_t> const &data);
	stats_t stats_max(sequence<stats_t> const &data);
	numeric_t numeric_max(sequence<numeric_t> const &data);
	numeric_t stats_median(sequence<stats_t> const &data);
	numeric_t numeric_median(sequence<numeric_t> const &data);
	stats_t stats_mode(sequence<stats_t> const &data);
	numeric_t numeric_mode(sequence<numeric_t> const &data);
	numeric_t stats_mean(sequence<stats_t> const &data);
	numeric_t numeric_mean(sequence<numeric_t> const &data);
	stats_t stats_range(sequence<stats_t> const &data);
	numeric_t numeric_range(sequence<numeric_t> const &data);
	numeric_t stats_sum(sequence<stats_t> const &data);
	numeric_t numeric_sum(sequence<numeric_t> const &data);
	numeric_t stats_var(sequence<stats_t> const &data);
	numeric_t numeric_var(sequence<numeric_t> const &data);
	numeric_t stats_var_pop(sequence<stats_t> const &data);
	numeric_t numeric_var_pop(sequence<numeric_t> const &data);
	numeric_t stats_stddev(sequence<stats_t> const &data);
	numeric_t numeric_stddev(sequence<numeric_t> const &data);
	numeric_t stats_stddev_pop(sequence<stats_t> const &data);
	numeric_t numeric_stddev_pop(sequence<numeric_t> const &data);
	numeric_t stats_correlation(sequence<stats_t> const &xdata,
	 sequence<stats_t> const &ydata);
	numeric_t numeric_correlation(sequence<numeric_t> const &xdata,
	 sequence<numeric_t> const &ydata);
	numeric_t stats_numeric_correlation(sequence<stats_t> const &xdata,
	 sequence<numeric_t> const &ydata);
	numeric_t numeric_stats_correlation(sequence<numeric_t> const &xdata,
	 sequence<stats_t> const &ydata);
	numeric_t stats_numeric_correlation(sequence<stats_t> const &xdata,
	 sequence<numeric_t> const &ydata);
	linear_regression *stats_find_linear_regression(
	 sequence<stats_t> const &xdata, sequence<stats_t> const &ydata);
	linear_regression *numeric_find_linear_regression(
	 sequence<numeric_t> const &xdata, sequence<numeric_t> const &ydata);
	linear_regression *stats_numeric_find_linear_regression(
	 sequence<stats_t> const &xdata, sequence<numeric_t> const &ydata);
	linear_regression *numeric_stats_find_linear_regression(
	 sequence<numeric_t> const &xdata, sequence<stats_t> const &ydata);

}
