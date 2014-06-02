#pragma once

#include "defs.hpp"
#include <cstddef>

namespace landsat
{
	typedef double numeric_t;
	typedef sequence<pixel_t> numeric_seq;
	typedef array<pixel_t> numeric_array;

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

	numeric_t min(numeric_seq const &data);
	numeric_t max(numeric_seq const &data);
	numeric_t median(numeric_seq const &data);
	numeric_t mode(numeric_seq const &data);
	numeric_t mean(numeric_seq const &data);
	numeric_t range(numeric_seq const &data);
	numeric_t sum(numeric_seq const &data);
	numeric_t var(numeric_seq const &data);
	numeric_t var_pop(numeric_seq const &data);
	numeric_t stddev(numeric_seq const &data);
	numeric_t stddev_pop(numeric_seq const &data);
	numeric_t correlation(numeric_seq const &xdata, numeric_seq const &ydata);
	linear_regression *find_linear_regression(numeric_seq const &xdata, numeric_seq const &ydata);

}
