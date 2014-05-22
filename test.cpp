#include "stats.hpp"
#include "types.hpp"
#include <iostream>
#include <cstddef>
#include <iomanip>

using landsat::numeric_t;
using landsat::numeric_array;

void print_arr(numeric_array &data);

int main(int argc, char **argv)
{
	numeric_t raw_data[] = {23., 45, -12, 12.4, 94, 294, 934, 39424, -2342, 0, -1, 34, 324.524, 34, 12, 36, 53, 18, 19, 20, 22, 21};
	numeric_array data(raw_data, sizeof(raw_data) / sizeof(numeric_t));
	numeric_t min = landsat::min(data);
	numeric_t max = landsat::max(data);
	numeric_t med = landsat::median(data);
	numeric_t range = landsat::range(data);
	numeric_t mode = landsat::mode(data);
	numeric_t mean = landsat::mean(data);
	std::cout << std::fixed << std::setprecision(2);
	std::cout << "Min: " << min << ", Max: " << max << ", Range: " << range << "," << std::endl;
	std::cout << "Mean: " << mean << ", Median: " << med << ", Mode: " << mode << std::endl;
	print_arr(data);
}

void print_arr(numeric_array &data)
{
	std::cout << "[";
	for (size_t i = 0; i < data.size(); i++) {
		if (i % 10 == 0) {
			std::cout << std::endl << "    ";
		}
		std::cout << data[i];
		if (i + 1 < data.size()) {
			std::cout << ",";
			if (i % 10 != 0) {
				std::cout << " ";
			}
		}
	}
	std::cout << std::endl << "]" << std::endl;
}
