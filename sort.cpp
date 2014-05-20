#include "sort.hpp"
#include "util.hpp"
#include <cstring>
#include <cstdlib>
#include <cassert>

namespace landsat
{

	static const size_t QS_MIN_SIZE = 20;
	
	static int compare_numerics(void const *x1, void const *x2);

	numeric_t *sort(numeric_t const *list, size_t size)
	{
		numeric_t *sorted = (numeric_t *)malloc(sizeof(numeric_t) * size);
		memcpy(sorted, list, sizeof(numeric_t) * size);
		if (size >= QS_MIN_SIZE) {
			quicksort(sorted, size);
		} else {
			insertion_sort(sorted, size);
		}
		return sorted;
	}

	void quicksort(numeric_t *list, size_t size)
	{
		qsort(list, size, sizeof(numeric_t), &compare_numerics);
	}

	void insertion_sort(numeric_t *list, size_t size)
	{
		size_t i, j;
		for (i = 1; i < size; i++) {
			j = i;
			while (j > 0 && list[j - 1] > list[j]) {
				swap(list + j, list + j - 1);
				j--;
			}
		}
	}

	static int compare_numerics(void const *x1, void const *x2)
	{
		numeric_t *n1 = (numeric_t *)x1;
		numeric_t *n2 = (numeric_t *)x2;
		return *n1 - *n2;
	}
}

