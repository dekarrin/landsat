#pragma once

#include "types.hpp"
#include <cstddef>

namespace landsat
{

	numeric_t min(numeric_t const *data, size_t size);
	numeric_t max(numeric_t const *data, size_t size);
	numeric_t median(numeric_t const *data, size_t size);
	numeric_t mode(numeric_t const *data, size_t size);
	numeric_t mean(numeric_t const *data, size_t size);
	numeric_t range(numeric_t const *data, size_t size);

}
