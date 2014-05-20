#include "stats.hpp"
#include "types.hpp"
#include <cstdlib>
#include <cstdio>

void printout(double data[], size_t size);

int main(int argc, char **argv)
{
	size_t SIZE = 22;
	numeric_t data[] = {23., 45, -12, 12.4, 94, 294, 934, 39424, -2342, 0, -1, 34, 324.524, 34, 12, 36, 53, 18, 19, 20, 22, 21};
	numeric_t min = landsat::min(data, SIZE);
	numeric_t max = landsat::max(data, SIZE);
	numeric_t med = landsat::median(data, SIZE);
	numeric_t range = landsat::range(data, SIZE);
	numeric_t mode = landsat::mode(data, SIZE);
	numeric_t mean = landsat::mean(data, SIZE);
	printf("[%.2f, %.2f] = %.2f, %.2f/%.2f, x%.2f\n", min, max, range, mean, med, mode);
	printout(data, SIZE);
}

void printout(double data[], size_t size)
{
	for (size_t i = 0; i < size; i++) {
		printf("%.2f", data[i]);
		if (i + 1 < size) {
			printf(", ");
		}
	}
	printf("\n");
}
