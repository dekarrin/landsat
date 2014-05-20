#include "stats.hpp"
#include "sort.hpp"
#include <map>
#include <cstdlib>

namespace landsat {

	numeric_t min(numeric_t const *data, size_t size)
	{
		numeric_t mn = data[0];
		for (size_t i = 0; i < size; i++) {
			if (mn > data[i]) {
				mn = data[i];
			}
		}
		return mn;
	}

	numeric_t max(numeric_t const *data, size_t size)
	{
		numeric_t mx = data[0];
		for (size_t i = 0; i < size; i++) {
			if (mx < data[i]) {
				mx = data[i];
			}
		}
		return mx;
	}

	numeric_t median(numeric_t const *data, size_t size)
	{
		numeric_t *sorted = sort(data, size);
		numeric_t med = sorted[size / 2];
		if (size % 2 == 0) {
			numeric_t med2 = sorted[(size / 2) - 1];
			med = ((med + med2) / 2);
		}
		free(sorted);
		return med;
	}

	numeric_t mode(numeric_t const *data, size_t size)
	{
		std::map<numeric_t, int> counts;
		for (size_t i = 0; i < size; i++) {
			if (counts.count(data[i]) == 0) {
				counts[data[i]] = 0;
			}
			counts[data[i]]++;
		}
		int mode_count = 0;
		numeric_t mode;
		std::map<numeric_t, int>::const_iterator iter;
		for (iter = counts.begin(); iter != counts.end(); iter++) {
			numeric_t element = iter->first;
			int count = iter->second;
			if (count > mode_count) {
				mode = element;
				mode_count = count;
			}
		}
		return mode;
	}

	numeric_t mean(numeric_t const *data, size_t size)
	{
		numeric_t sum = data[0];
		for (size_t i = 1; i < size; i++) {
			sum += data[i];
		}
		numeric_t avg = sum / size;
		return avg;
	}

	numeric_t range(numeric_t const *data, size_t size)
	{
		numeric_t mn = min(data, size);
		numeric_t mx = max(data, size);
		numeric_t rng = mx - mn;
		return rng;
	}

}

