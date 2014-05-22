#include "sort.hpp"
#include <cstdlib>

namespace landsat
{

	static const size_t QS_MIN_SIZE = 20;
	
	static int compare_numerics(void const *x1, void const *x2);

	numeric_array *sort_const(numeric_array const &list)
	{
		numeric_array *sorted = new numeric_array(list);
		sort(*sorted);
		return sorted;
	}

	void sort(numeric_array &list)
	{
		if (list.size() >= QS_MIN_SIZE) {
			quicksort(list);
		} else {
			insertion_sort(list);
		}
	}

	void quicksort(numeric_array &list)
	{
		qsort(list.data(), list.size(), list.element_size(), &compare_numerics);
	}

	void insertion_sort(numeric_array &list)
	{
		size_t i, j;
		for (i = 1; i < list.size(); i++) {
			j = i;
			while (j > 0 && list[j - 1] > list[j]) {
				list.swap(j, j - 1);
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

