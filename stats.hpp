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

}
