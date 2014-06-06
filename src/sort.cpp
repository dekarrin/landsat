#include "sort.hpp"
#include <cstdlib>

namespace landsat
{

	static const size_t QS_MIN_SIZE = 20;
	
	static int compare_numerics(void const *x1, void const *x2);

	array<numeric_t> *sort_const(array<numeric_t> const &list)
	{
		array<numeric_t> *sorted = new array<numeric_t>(list);
		sort(*sorted);
		return sorted;
	}

	void sort(array<numeric_t> &list)
	{
		if (list.size() >= QS_MIN_SIZE) {
			quicksort(list);
		} else {
			insertion_sort(list);
		}
	}

	void quicksort(array<numeric_t> &list)
	{
		qsort(list.data(), list.size(), list.element_size(), &compare_numerics);
	}

	void insertion_sort(array<numeric_t> &list)
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

