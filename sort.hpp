#pragma once

#include "types.hpp"
#include <cstddef>

namespace landsat
{
	GENERIC *sort(T const *list, size_t size);
	GENERIC_V quicksort(T *list, size_t size);
//	GENERIC_V insertion_sort(T *list, size_t size);
	void insertion_sort(double *list, size_t size);
}
