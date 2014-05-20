#pragma once

#include "types.hpp"
#include <cstddef>

namespace landsat
{

	GENERIC min(T const *data, size_t size);
	GENERIC max(T const *data, size_t size);
	GENERIC median(T const *data, size_t size);
	GENERIC mode(T const *data, size_t size);
	GENERIC mean(T const *data, size_t size);
	GENERIC range(T const *data, size_t size);

}
