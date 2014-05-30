#pragma once

#include "defs.hpp"
#include <cstddef>

namespace landsat
{
	typedef double numeric_t;
	typedef array<numeric_t> numeric_array;

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

	numeric_t min(numeric_array const &data);
	numeric_t max(numeric_array const &data);
	numeric_t median(numeric_array const &data);
	numeric_t mode(numeric_array const &data);
	numeric_t mean(numeric_array const &data);
	numeric_t range(numeric_array const &data);
	numeric_t sum(numeric_array const &data);
	numeric_t var(numeric_array const &data);
	numeric_t var_pop(numeric_array const &data);
	numeric_t stddev(numeric_array const &data);
	numeric_t stddev_pop(numeric_array const &data);
	numeric_t correlation(numeric_array const &xdata, numeric_array const &ydata);
	linear_regression *find_linear_regression(numeric_array const &xdata, numeric_array const &ydata);

}
