#include "cells.hpp"

#include "stats.hpp"

namespace landsat
{

	static numeric_t get_cell_slope(grid<pixel_t> const &red, grid<pixel_t> const &nir, size_t cell_size);

	static numeric_t get_cell_slope(grid<pixel_t> const &red, grid<pixel_t> const &red, size_t cell_size)
	{
		rect<size_t> sub = {0, 0, cell_size, cell_size};
		size_t xCount = data.width() / cell_size;
		size_t yCount = data.height() / cell_size;
		numeric_array red_avg(xCount * yCount);
		numeric_array nir_avg(xCount * yCount);
		numeric_t *red_avg_ptr = red_avg.data();
		numeric_t *nir_avg_ptr = nir_avg.data();
		for (sub.x = 0; sub.x + (sub.width - 1) < data.width(); sub.x += cell_size) {
			for (sub.y = 0; sub.y + (sub.height - 1) < data.height(); sub.y += cell_size) {
				grid<pixel_t> sub_red(&red, sub);
				grid<pixel_t> sub_nir(&nir, sub);
				*red_avg_ptr = mean(sub_red);
				*nir_avg_ptr = 
			}			
		}
	}

}
