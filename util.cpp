#include "util.hpp"

namespace landsat
{
	void swap(numeric_t &x1, numeric_t &x2)
	{
		numeric_t t = x1;
		x1 = x2;
		x2 = t;
	}
}
