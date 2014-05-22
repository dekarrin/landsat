#pragma once

#include "types.hpp"
#include <cstddef>

namespace landsat
{

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
