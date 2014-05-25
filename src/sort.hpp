#pragma once

#include "types.hpp"
#include <cstddef>

namespace landsat
{
	numeric_array *sort_const(numeric_array const &list);
	void sort(numeric_array &list);
	void quicksort(numeric_array &list);
	void insertion_sort(numeric_array &list);
}
