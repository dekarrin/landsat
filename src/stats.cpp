#include "stats.hpp"
#include "sort.hpp"
#include <map>
#include <cstddef>
#include <cmath>

namespace landsat {

	bool check_horizontal_fit(linear_regression &reg, numeric_seq const &xdata, numeric_seq const &ydata);
	numeric_t residual_sum_squares(linear_eq const &model, numeric_seq const &xdata, numeric_seq const &ydata);
	numeric_t total_sum_squares(numeric_seq const &data);

	stats_t stats_min(sequence<stats_t> const &data)
	{
		data.reset();
		stats_t mn = data.next();
		while (data.has_next()) {
			stats_t n = data.next();
			if (mn > n) {
				mn = n;
			}
		}
		return mn;
	}

	numeric_t min(sequence<numeric_t> const &data)
	{
		data.reset();
		numeric_t mn = data.next();
		while (data.has_next()) {
			numeric_t n = data.next();
			if (mn > n) {
				mn = n;
			}
		}
		return mn;
	}

	stats_t max(seqeunce<stats_t> const &data)
	{
		data.reset();
		stats_t mx = data.next();
		while (data.has_next()) {
			stats_t n = data.next();
			if (mx < n) {
				mx = n;
			}
		}
		return mx;
	}

	numeric_t max(seqeunce<numeric_t> const &data)
	{
		data.reset();
		numeric_t mx = data.next();
		while (data.has_next()) {
			stats_t n = data.next();
			if (mx < n) {
				mx = n;
			}
		}
		return mx;
	}

	numeric_t median(numeric_seq const &data)
	{
		data.reset();
		numeric_array copy(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			copy[i] = data[i];
		}
		numeric_array *sorted = sort_const(copy);
		numeric_t med = (*sorted)[sorted->size() / 2];
		if (sorted->size() % 2 == 0) {
			numeric_t med2 = sorted->data()[(sorted->size() / 2) - 1];
			med = ((med + med2) / 2);
		}
		delete sorted;
		return med;
	}

	stats_t mode(numeric_seq const &data)
	{
		data.reset();
		std::map<stats_t, int> counts;
		while (data.has_next()) {
			numeric_t n = data.next();
			if (counts.count(n) == 0) {
				counts[n] = 0;
			}
			counts[n]++;
		}
		int mode_count = 0;
		stats_t mode;
		std::map<stats_t, int>::const_iterator iter;
		for (iter = counts.begin(); iter != counts.end(); iter++) {
			stats_t element = iter->first;
			int count = iter->second;
			if (count > mode_count) {
				mode = element;
				mode_count = count;
			}
		}
		return mode;
	}

	numeric_t mean(numeric_seq const &data)
	{
		data.reset();
		numeric_t avg = 0;
		while (data.has_next()) {
			numeric_t n = data.next();
			avg += n / data.size();
		}
		return avg;
	}

	stats_t range(numeric_seq const &data)
	{
		stats_t mn = min(data);
		stats_t mx = max(data);
		stats_t rng = mx - mn;
		return rng;
	}

	numeric_t sum(numeric_seq const &data)
	{
		data.reset();
		numeric_t sm = 0;
		while (data.has_next()) {
			sm += data.next();
		}
		return sm;
	}

	numeric_t var(numeric_seq const &data)
	{
		numeric_t avg = mean(data);
		numeric_t varsum = 0;
		data.reset();
		while (data.has_next()) {
			varsum += pow(((numeric_t)data.next()) - avg, 2);
		}
		numeric_t v = varsum / (data.size() - 1);
		return v;
	}

	numeric_t var_pop(numeric_seq const &data)
	{
		numeric_t avg = mean(data);
		numeric_t varsum = 0;
		data.reset();
		while (data.has_next()) {
			varsum += pow(((numeric_t)data.next()) - avg, 2);
		}
		numeric_t v = varsum / data.size();
		return v;
	}

	numeric_t stddev(numeric_seq const &data)
	{
		return sqrt(var(data));
	}

	numeric_t stddev_pop(numeric_seq const &data)
	{
		return sqrt(var_pop(data));
	}

	numeric_t correlation(numeric_seq const &xdata, numeric_seq const &ydata)
	{
		numeric_t x_sum, y_sum, xx_sum, yy_sum, xy_sum;
		x_sum = y_sum = xx_sum = yy_sum = xy_sum = 0;
		size_t highsize = (xdata.size() > ydata.size()) ? xdata.size() : ydata.size();
		for (size_t i = 0; i < highsize; i++) {
			numeric_t x, y;
			x = (xdata.size() < highsize) ? 0 : xdata[i];
			y = (ydata.size() < highsize) ? 0 : ydata[i];
			x_sum += x;
			y_sum += y;
			xx_sum += x * x;
			yy_sum += y * y;
			xy_sum += x * y;
		}
		numeric_t r_top = (((numeric_t) highsize) * xy_sum) - (x_sum * y_sum);
		numeric_t r_bot_x = sqrt((((int) highsize) * xx_sum) - (x_sum * x_sum));
		numeric_t r_bot_y = sqrt((((int) highsize) * yy_sum) - (y_sum * y_sum));
		numeric_t r = r_top / (r_bot_x * r_bot_y);
		return r;
	}

	linear_regression *find_linear_regression(numeric_seq const &xdata, numeric_seq const &ydata)
	{
		// uses ordinary least squares
			
		linear_regression *regression = new linear_regression;
		// first test the special case of a horizontal fit:
		if (!check_horizontal_fit(*regression, xdata, ydata)) {
			numeric_t r = correlation(xdata, ydata);
			numeric_t xm = mean(xdata);
			numeric_t ym = mean(ydata);
			numeric_t xs = stddev(xdata);
			numeric_t ys = stddev(ydata);
			regression->eq.slope = r * (ys / xs);
			regression->eq.intercept = ym - regression->eq.slope * xm;
			numeric_t ss_res = residual_sum_squares(regression->eq, xdata, ydata);
			numeric_t ss_tot = total_sum_squares(ydata);
			regression->r2 = 1 - (ss_res / ss_tot);
		}
		return regression;
	}

	bool check_horizontal_fit(linear_regression &reg, numeric_seq const &xdata, numeric_seq const &ydata)
	{
		bool is_horz = true;
		stats_t initial = ydata[0];
		for (size_t i = 1; i < ydata.size(); i++) {
			if (ydata[i] != initial) {
				is_horz = false;
				break;
			}
		}
		if (is_horz) {
			// if at least two data points are different, we have a horizontal correlation
			bool x_is_same = true;
			stats_t initialx = xdata[0];
			for (size_t i = 1; i < xdata.size(); i++) {
				if (xdata[i] != initial) {
					x_is_same = false;
					break;
				}
			}
			if (!x_is_same) {
				reg.r2 = 1;
				reg.eq.slope = 0;
				reg.eq.intercept = ydata[0];
			} else {
				// the data collapses to one point; impossible to regress
				reg.r2 = 0.0/0.0;
				reg.eq.slope = 0.0/0.0;
				reg.eq.intercept = 0.0/0.0;
			}
		}
		return is_horz;
	}

	numeric_t residual_sum_squares(linear_eq const &model, numeric_seq const &xdata, numeric_seq const &ydata)
	{
		numeric_t sm = 0;
		size_t highsize = (xdata.size() > ydata.size()) ? xdata.size() : ydata.size();
		for (size_t i = 0; i < highsize; i++) {
			numeric_t x, y;
			x = (xdata.size() < highsize) ? 0 : xdata[i];
			y = (ydata.size() < highsize) ? 0 : ydata[i];
			numeric_t res = y - (model.slope * x + model.intercept);
			sm += pow(res, 2);
		}
		return sm;
	}

	numeric_t total_sum_squares(numeric_seq const &data)
	{
		numeric_t avg = mean(data);
		numeric_t sm = 0;
		for (size_t i = 0; i < data.size(); i++) {
			numeric_t sq = pow(((numeric_t)data[i]) - avg, 2);
			sm += sq;
		}
		return sm;
	}
}

