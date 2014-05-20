#include "stats.hpp"
#include "sort.hpp"
#include <map>

namespace landsat {

	GENERIC min(T const *data, size_t size)
	{
		T mn = data[0];
		for (int i = 0; i < size; i++) {
			if (mn > data[i]) {
				mn = data[i];
			}
		}
		return mn;
	}

	GENERIC max(T const *data, size_t size)
	{
		T mx = data[0];
		for (int i = 0; i < size; i++) {
			if (mx < data[i]) {
				mx = data[i];
			}
		}
		return mx;
	}

	GENERIC median(T const *data, size_t size)
	{
		T *sorted = sort(data, size);
		T med = sorted[size / 2];
		if (size % 2 == 0) {
			T med2 = sorted[(size / 2) - 1];
			med = ((med + med2) / 2);
		}
		free(sorted);
		return med;
	}

	GENERIC mode(T const *data, size_t size)
	{
		std::map<T, int> counts;
		for (int i = 0; i < size; i++) {
			if (counts.count(data[i]) == 0) {
				counts[data[i]] = 0;
			}
			counts[data[i]]++;
		}
		int mode_count = 0;
		T mode;
		std::map<T, int>::const_iterator iter;
		for (iter = counts.begin(); iter != counts.end(); iter++) {
			T element = iter->first;
			int count = iter->second;
			if (count > mode_count) {
				mode = element;
			}
		}
		return mode;
	}

	GENERIC mean(T const *data, size_t size)
	{
		T sum = data[0];
		for (int i = 1; i < size; i++) {
			sum += data[i];
		}
		T avg = sum / size;
		return avg;
	}

	GENERIC range(T const *data, size_t size)
	{
		T min = min(data, size);
		T max = max(data, size);
		T rng = max - min;
		return rng;
	}

}

