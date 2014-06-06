namespace landsat
{

	cell_regression_stats *get_cell_regression_stats(
	 grid<pixel_t> const &red, grid<pixel_t> const &nir, size_t cell_size);
	array<cell_regression_stats> *get_all_cell_regression_stats(
	 grid<pixel_t> const &red, grid<pixel_t> const &nir);


	cell_regression_stats *get_cell_regression_stats(
	 grid<pixel_t> const &red, grid<pixel_t> const &nir, size_t size)
	{
		size_t output_block = 100;
		rect<size_t> sub = {0, 0, size, size};
		size_t xCount = red.width() / size;
		size_t yCount = red.height() / size;
		IF_NORMAL(std::cout << yCount << " rows" << std::endl);
		size_t cell_count = xCount * yCount;
		array<numeric_t> red_avg(cell_count);
		array<numeric_t> nir_avg(cell_count);
		array<bool> goodness(cell_count);
		size_t good_count = 0;
		bool *goodness_ptr = goodness.data();
		numeric_t *red_avg_ptr = red_avg.data();
		numeric_t *nir_avg_ptr = nir_avg.data();
		for (sub.x = 0; sub.x + (sub.width - 1) < red.width();
		 sub.x += size) {
			for (sub.y = 0;
			 sub.y + (sub.height - 1) < red.height();
			 sub.y += size) {
				const grid<pixel_t> sub_red(
				 const_cast<grid<pixel_t>*>(&red), sub);
				const grid<pixel_t> sub_nir(
				 const_cast<grid<pixel_t>*>(&nir), sub);
				if (is_good_data(sub_red, sub_nir)) {
					*red_avg_ptr = stats_mean(sub_red);
					*nir_avg_ptr = stats_mean(sub_nir);
					*goodness_ptr = true;
					good_count++;
				} else {
					IF_VERBOSE(std::cout << "Bad sector: ("
					 );
					IF_VERBOSE(std::cout << sub.x << ", ");
					IF_VERBOSE(std::cout << sub.y << ") ");
					*goodness_ptr = false;
				}
				red_avg_ptr++;
				nir_avg_ptr++;
				goodness_ptr++;
			}
			if ((sub.y / size) % output_block == 0) {
				IF_NORMAL(std::cout << "\ranalyzing rows ");
				IF_NORMAL(std::cout << "starting at ");
				IF_NORMAL(std::cout << (sub.y / size));
				IF_NORMAL(std::cout << "...");
				IF_NORMAL(std::cout.flush());
			}
		}
		IF_NORMAL(std::cout << std::endl);
		IF_VERBOSE(std::cout << "Found " << good_count << " good ");
		IF_VERBOSE(std::cout << "sectors out of " << cell_count);
		IF_VERBOSE(std::cout << std::endl);
		array<numeric_t> good_red_data(good_count);
		array<numeric_t> good_nir_data(good_count);
		size_t good_data_cur = 0;
		for (size_t i = 0; i < cell_count; i++) {
			if (goodness[i]) {
				good_red_data[good_data_cur] = red_avg[i];
				good_nir_data[good_data_cur] = nir_avg[i];
				good_data_cur++;
			}
		}
		linear_regression *reg = numeric_find_linear_regression(
		 good_red_data, good_nir_data);
		cell_regression_stats *stats = new cell_regression_stats;
		stats->cell_size = size;
		stats->regression = *reg;
		delete reg;
		return stats;
	}

	array<cell_regression_stats> *get_all_cell_regression_stats(
	 const grid<pixel_t> &red, const grid<pixel_t> &nir)
	{
		size_t cell_base = 2;
		size_t stats_count = (size_t) ((log(red.width()) /
		 log(cell_base)) - 1);
		array<cell_regression_stats> *all_stats =
		 new array<cell_regression_stats>(stats_count);
		size_t pos = 0;
		for (size_t cell_size = cell_base;
		 cell_size * cell_base < red.height(); cell_size *= cell_base) {
			IF_NORMAL(std::cout << "Checking cell size ");
			IF_NORMAL(std::cout << cell_size << "..." << std::endl);
			cell_regression_stats *stats =
			 get_cell_regression_stats(red, nir, cell_size);
			 (*all_stats)[pos++] = *stats;
			delete stats;
		}
		return all_stats;
	}

}
