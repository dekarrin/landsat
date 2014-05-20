#pragma once

#include "types.hpp"
#include <cstddef>

namespace landsat
{
	numeric_t *sort(numeric_t const *list, size_t size);
	void quicksort(numeric_t *list, size_t size);
	void insertion_sort(numeric_t *list, size_t size);
}
