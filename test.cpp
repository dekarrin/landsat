#include "sort.hpp"
#include <cstdlib>
#include <cstdio>

void printout(double data[], size_t size);

int main(int argc, char **argv)
{
	size_t SIZE = 12;
	double data[] = {23., 45, -12, 12.4, 94, 294, 934, 39424, -2342, 0, -1, 34};
	landsat::insertion_sort(data, SIZE);
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
