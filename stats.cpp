#include "stats.hpp"
#include "sort.hpp"
#include <map>
#include <cstddef>

namespace landsat {

	numeric_t min(numeric_array const &data)
	{
		numeric_t mn = data[0];
		for (size_t i = 0; i < data.size(); i++) {
			if (mn > data[i]) {
				mn = data[i];
			}
		}
		return mn;
	}

	numeric_t max(numeric_array const &data)
	{
		numeric_t mx = data[0];
		for (size_t i = 0; i < data.size(); i++) {
			if (mx < data[i]) {
				mx = data[i];
			}
		}
		return mx;
	}

	numeric_t median(numeric_array const &data)
	{
		numeric_array *sorted = sort_const(data);
		numeric_t med = (*sorted)[sorted->size() / 2];
		if (sorted->size() % 2 == 0) {
			numeric_t med2 = sorted->data()[(sorted->size() / 2) - 1];
			med = ((med + med2) / 2);
		}
		delete sorted;
		return med;
	}

	numeric_t mode(numeric_array const &data)
	{
		std::map<numeric_t, int> counts;
		for (size_t i = 0; i < data.size(); i++) {
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

	numeric_t mean(numeric_array const &data)
	{
		numeric_t sum = data[0];
		for (size_t i = 1; i < data.size(); i++) {
			sum += data[i];
		}
		numeric_t avg = sum / data.size();
		return avg;
	}

	numeric_t range(numeric_array const &data)
	{
		numeric_t mn = min(data);
		numeric_t mx = max(data);
		numeric_t rng = mx - mn;
		return rng;
	}

}

