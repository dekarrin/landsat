#pragma once

#include "defs.hpp"
#include "stats.hpp"
#include <cstddef>

namespace landsat
{
	array<numeric_t> *sort_const(array<numeric_t> const &list);
	void sort(array<numeric_t> &list);
	void quicksort(array<numeric_t> &list);
	void insertion_sort(array<numeric_t> &list);
}
