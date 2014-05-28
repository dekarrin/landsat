#include "data.hpp"
#include <cstring>
#include <iostream>
#include <tiffio.h>

namespace landsat
{

	struct tiff_t
	{
		char *filename;
		TIFF *handle;
		uint16 width;
		uint16 height;
		uint16 bitdepth;
		uint16 channels;
		uint16 planarconfig;
	};

	grid<pixel_t> *read_tiff(const tiff_t *image, tsample_t sample);
	tiff_t *open_tiff(const char *filename);
	void close_tiff(const tiff_t *image);

	grid<pixel_t> *get_data(const char *filename)
	{
		tiff_t *image = open_tiff(filename);
		grid<pixel_t> *data = read_tiff(image, 0);
		close_tiff(image);
		delete image;
		return data;
	}

	grid<pixel_t> *read_tiff(const tiff_t *image, tsample_t sample)
	{
		grid<pixel_t> *img_data = new grid<pixel_t>(image->width, image->height);
		tsize_t size = TIFFScanlineSize(image->handle);
		tdata_t buffer = _TIFFmalloc(size);
		for (uint16 i = 0; i < image->height; i++) {
			TIFFReadScanline(image->handle, buffer, i, sample);
			_TIFFmemcpy(img_data->data + (i * image->width), buffer, (image->bitdepth / 8) * image->width);
		}
		_TIFFfree(buffer);
		return img_data;
	}

	void close_tiff(const tiff_t *image)
	{
		TIFFClose(image->handle);
	}

	tiff_t *open_tiff(const char *filename)
	{
		tiff_t *image = new tiff_t;
		image->filename = new char[strlen(filename)];
		strcpy(image->filename, filename);
		TIFFErrorHandler old = TIFFSetWarningHandler(NULL);
		image->handle = TIFFOpen(filename, "r");
		TIFFSetWarningHandler(old);
		uint16 buffer;
		TIFFGetField(image->handle, TIFFTAG_IMAGEWIDTH, &buffer);
		image->width = buffer;
		TIFFGetField(image->handle, TIFFTAG_IMAGELENGTH, &buffer);
		image->height = buffer;
		TIFFGetField(image->handle, TIFFTAG_BITSPERSAMPLE, &buffer);
		image->bitdepth = buffer;
		TIFFGetField(image->handle, TIFFTAG_SAMPLESPERPIXEL, &buffer);
		image->channels = buffer;
		TIFFGetField(image->handle, TIFFTAG_PLANARCONFIG, &buffer);
		image->planarconfig = buffer;
		return image;
	}
}
