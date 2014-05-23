#include "data.hpp"
#include <iostream>
#include <fstream>

#define ERR_ARG_COUNT 1
#define ERR_BAD_FILE 2

int main(int argc, char **argv)
{
	if (argc < 2) {
		std::cerr << "usage: " << argv[0] << " [tif-image]" << std::endl;
		return ERR_ARG_COUNT;
	} else {
		const char *file = argv[1];
		std::ifstream tiff_file(file);
		bool exists = tiff_file.good();
		tiff_file.close();
		if (!exists) {
			std::cerr << "could not open '" << argv[1] << "'" << std::endl;
			return ERR_BAD_FILE;
		} else {
			landsat::get_data(file);
			return 0;
		}
	}
}
