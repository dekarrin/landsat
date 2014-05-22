#include "data.hpp"
#include <iostream>
#include <tiffio.h>

typedef unsigned short pixel_t;

void get_data(const char *filename)
{
	TIFFErrorHandler old_handler = TIFFSetWarningHandler(NULL);
	TIFF *file = TIFFOpen("/home/dekarrin/red.tif", "r");
	TIFFSetWarningHandler(old_handler);
	uint16 sto = 0;
	uint16 bits = 0;
	uint16 samples;
	uint16 width;
	uint16 height;
	uint16 count = 0;
	TIFFGetField(file, TIFFTAG_BITSPERSAMPLE, &sto);
	bits = sto;
	TIFFGetField(file, TIFFTAG_SAMPLESPERPIXEL, &sto);
	samples = sto;
	TIFFGetField(file, TIFFTAG_IMAGELENGTH, &sto);
	height = sto;
	TIFFGetField(file, TIFFTAG_IMAGEWIDTH, &sto);
	width = sto;
	std::cout << bits << "\n";
	std::cout << samples << "\n";
	std::cout << width << "x" << height << std::endl;
	size_t pixel(bits / 8 * samples);


	TIFFClose(file);
}
