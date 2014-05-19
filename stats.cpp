#include "stats.h"
#include <cstring>

#define SINGLE "template <typename T> T"
#define MULTI "template <typename T> T *"
#define ARGS "T const * data, size_t size"

stats {

	MULTI sort(ARGS);
	
	SINGLE min(ARGS)
	{
		T mn = data[0];
		for (int i = 0; i < size; i++) {
			if (mn > data[i]) {
				mn = data[i];
			}
		}
		return mn;
	}

	SINGLE max(ARGS)
	{
		T mx = data[0];
		for (int i = 0; i < size; i++) {
			if (mx < data[i]) {
				mx = data[i];
			}
		}
		return mx;
	}

	SINGLE median(ARGS)
	{
		
		if (size % 2 == 0) {
			
		}
	}

	template <typename T>
	void quick_sort(T * list, size_t size);

	MULTI sort(ARGS)
	{
		T * sorted = malloc(sizeof(T) * size);
		memcpy(sorted, data, sizeof(T) * size);
		quick_sort(sorted, size);
		return sorted;
	}

	template <typename T>
	void quick_sort(T * list, size_t size)
	{
		if (size <= 1) {
			return;
		}
		T first, last, mid;
		size_t pivot;
		first = list[0];
		last = list[size - 1];
		mid = list[size / 2];
		if ((med <= first && first <= last) || (last <= first && first <= med)) {
			pivot = 0;
		} else if ((med <= last && last <= first) || (first <= last && last <= med)) {
			pivot = size - 1;
		} else {
			pivot = size / 2;
		}
		

		T * left, * right;
		size_t middle = size / 2;
		left = list;
		right = list + (size / 2);
		size_t left_size = size / 2;
		size_t right_size = size - (size / 2);
		merge_sort(left, left_size);
		merge_sort(right, right_size); 
	}
	

}


