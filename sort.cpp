#include "sort.hpp"
#include "util.hpp"
#include <cstring>
#include <cstdlib>
#include <cassert>

namespace landsat
{

	static const size_t QS_MIN_SIZE = 20;

	GENERIC_V static qs_recurse(T *left, T *right);
	GENERIC_V static qs_partition(T *left, T *right, T **pivot);
	GENERIC static *qs_pivot(T *left, T *right);

	GENERIC *sort(T const *list, size_t size)
	{
		T *sorted = malloc(sizeof(T) * size);
		memcpy(sorted, list, sizeof(T) * size);
		if (size >= QS_MIN_SIZE) {
			quicksort(sorted, size);
		} else {
			insertion_sort(sorted, size);
		}
		return sorted;
	}

	GENERIC_V quicksort(T *list, size_t size)
	{
		qs_recurse(list, list + size - 1);
	}
/*
	GENERIC_V insertion_sort(T *list, size_t size)
	{
		T tmp;
		int i, j;
		for (i = 1; i < size; i++) {
			j = i;
			while (j > 0 && list[j - 1] > list[j]) {
				swap(list + j, list + j - 1);
				j--;
			}
		}
	}
*/
	void insertion_sort(double *list, size_t size)
	{
		double tmp;
		int i, j;
		for (i = 1; i < size; i++) {
			j = i;
			while (j > 0 && list[j - 1] > list[j]) {
				swap(list + j, list + j - 1);
				j--;
			}
		}
	}
	GENERIC_V qs_recurse(T *left, T *right)
	{
		if (left < right) {
			T *pivot = qs_pivot(left, right);
			qs_partition(left, right, &pivot);
			assert(pivot-1 >= left);
			assert(pivot+1 <= right);
			quicksort_sub(left, pivot-1);
			quicksort_sub(pivot+1, right);
		}
	}

	GENERIC_V qs_partition(T *left, T *right, T **pivot)
	{
		T pivotVal = **pivot;
		swap(*pivot, right);
		*pivot = left;
		T *ptr;
		for (ptr = left; ptr < right; ptr++) {
			if (*ptr < pivotVal) {
				swap(ptr, *pivot);
				*pivot++;
			}
		}
		swap(*pivot, right);
	}

	GENERIC *qs_pivot(T *left, T *right)
	{
		T *pivot, *mid;
		mid = (right + left) / 2;
		// select the median of middle, left, and right for the pivot
		if ((*left <= *mid && *mid <= *right) || (*right <= *mid && *mid <= *left)) {
			pivot = mid;
		} else if ((*mid <= *left && *left <= *right) || (*right <= *left && *left <= *mid)) {
			pivot = left;
		} else {
			pivot = right;
		}
		return pivot;
	}

}

