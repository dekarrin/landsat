#pragma once

#include "defs.hpp"

namespace landsat
{
	typedef unsigned short pixel_t;

	grid<pixel_t> *get_data(const char *filename);
}

