#include "test.hpp"

#include <iostream>
#include <cstddef>
#include <iomanip>

using landsat::numeric_t;
using landsat::array;

int main(int argc, char **argv)
{
	numeric_t raw_data[] = {23., 45, -12, 12.4, 94, 294, 934, 394, -2342, 0,
	 -1, 34, 324.524, 34, 12, 36, 53, 18, 19, 20, 22, 21};
	array<numeric_t> datay(sizeof(raw_data) / sizeof(numeric_t));
	array<numeric_t> data(raw_data, sizeof(raw_data) / sizeof(numeric_t));
	for (size_t i = 0; i < datay.size(); i++) {
		datay[i] = i;
	}
	numeric_t min = landsat::numeric_min(data);
	numeric_t max = landsat::numeric_max(data);
	numeric_t med = landsat::numeric_median(data);
	numeric_t range = landsat::numeric_range(data);
	numeric_t mode = landsat::numeric_mode(data);
	numeric_t mean = landsat::numeric_mean(data);
	numeric_t sum = landsat::numeric_sum(data);
	numeric_t var = landsat::numeric_var(data);
	numeric_t var_pop = landsat::numeric_var_pop(data);
	numeric_t stddev = landsat::numeric_stddev(data);
	numeric_t stddev_pop = landsat::numeric_stddev_pop(data);
	numeric_t correlation = landsat::numeric_correlation(data, datay);
	landsat::linear_regression *reg =
	 landsat::numeric_find_linear_regression(data, datay);
	std::cout << std::fixed << std::setprecision(3);
	std::cout << "Min: " << min << std::endl;
	std::cout << "Max: " << max << std::endl;
	std::cout << "Range: " << range << std::endl;
	std::cout << "Mean: " << mean << std::endl;
	std::cout << "Median: " << med << std::endl;
	std::cout << "Mode: " << mode << std::endl;
	std::cout << "Sum: " << sum << std::endl;
	std::cout << "Variance: " << var << " / " << var_pop << std::endl;
	std::cout << "Standard deviation: " << stddev << " / " << stddev_pop
	 << std::endl;
	std::cout << "X-Y correlation: " << correlation << std::endl;
	std::cout << std::setprecision(8);
	std::cout << "Line of best fit: y = " << reg->eq.slope;
	if (reg->eq.intercept > 0) {
		std::cout << "x + " << reg->eq.intercept;
	} else if (reg->eq.intercept < 0) {
		std::cout << "x - " << (reg->eq.intercept * -1);
	}
	std::cout << ", R^2 = " << reg->r2 << std::endl;
	std::cout << std::setprecision(2);
	std::cout << std::endl << "X-data:" << std::endl;
	print_arr(data);
	std::cout << std::endl << "Y-data:" << std::endl;
	print_arr(datay);
	delete reg;
}

void print_arr(array<numeric_t> &data)
{
	std::cout << "[";
	for (size_t i = 0; i < data.size(); i++) {
		if (i % 10 == 0) {
			std::cout << std::endl << "    ";
		}
		std::cout << data[i];
		if (i + 1 < data.size()) {
			std::cout << ",";
			if (i % 10 != 9) {
				std::cout << " ";
			}
		}
	}
	std::cout << std::endl << "]" << std::endl;
}
