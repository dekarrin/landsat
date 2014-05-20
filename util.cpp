#include "util.hpp"

namespace landsat
{
	GENERIC_V swap(T *x1, T *x2)
	{
		T t = *x1;
		*x1 = *x2;
		*x2 = t;
	}
}
