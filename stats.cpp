#include "stats.hpp"
#include "sort.hpp"
#include <map>
#include <cstddef>
#include <cmath>

namespace landsat {

	numeric_t residual_sum_squares(linear_eq const &model, numeric_array const &xdata, numeric_array const &ydata);
	numeric_t total_sum_squares(numeric_array const &data);

	numeric_t min(numeric_array const &data)
	{
		numeric_t mn = data[0];
		for (size_t i = 0; i < data.size(); i++) {
			if (mn > data[i]) {
				mn = data[i];
			}
		}
		return mn;
	}

	numeric_t max(numeric_array const &data)
	{
		numeric_t mx = data[0];
		for (size_t i = 0; i < data.size(); i++) {
			if (mx < data[i]) {
				mx = data[i];
			}
		}
		return mx;
	}

	numeric_t median(numeric_array const &data)
	{
		numeric_array *sorted = sort_const(data);
		numeric_t med = (*sorted)[sorted->size() / 2];
		if (sorted->size() % 2 == 0) {
			numeric_t med2 = sorted->data()[(sorted->size() / 2) - 1];
			med = ((med + med2) / 2);
		}
		delete sorted;
		return med;
	}

	numeric_t mode(numeric_array const &data)
	{
		std::map<numeric_t, int> counts;
		for (size_t i = 0; i < data.size(); i++) {
			if (counts.count(data[i]) == 0) {
				counts[data[i]] = 0;
			}
			counts[data[i]]++;
		}
		int mode_count = 0;
		numeric_t mode;
		std::map<numeric_t, int>::const_iterator iter;
		for (iter = counts.begin(); iter != counts.end(); iter++) {
			numeric_t element = iter->first;
			int count = iter->second;
			if (count > mode_count) {
				mode = element;
				mode_count = count;
			}
		}
		return mode;
	}

	numeric_t mean(numeric_array const &data)
	{
		numeric_t avg = sum(data) / data.size();
		return avg;
	}

	numeric_t range(numeric_array const &data)
	{
		numeric_t mn = min(data);
		numeric_t mx = max(data);
		numeric_t rng = mx - mn;
		return rng;
	}

	numeric_t sum(numeric_array const &data)
	{
		numeric_t sm = 0;
		for (size_t i = 0; i < data.size(); i++) {
			sm += data[i];
		}
		return sm;
	}

	numeric_t var(numeric_array const &data)
	{
		numeric_t avg = mean(data);
		numeric_t varsum = 0;
		for (size_t i = 0; i < data.size(); i++) {
			varsum += pow(data[i] - avg, 2);
		}
		numeric_t v = varsum / (data.size() - 1);
		return v;
	}

	numeric_t var_pop(numeric_array const &data)
	{
		numeric_t avg = mean(data);
		numeric_t varsum = 0;
		for (size_t i = 0; i < data.size(); i++) {
			varsum += pow(data[i] - avg, 2);
		}
		numeric_t v = varsum / data.size();
		return v;
	}

	numeric_t stddev(numeric_array const &data)
	{
		return sqrt(var(data));
	}

	numeric_t stddev_pop(numeric_array const &data)
	{
		return sqrt(var_pop(data));
	}

	numeric_t correlation(numeric_array const &xdata, numeric_array const &ydata)
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
		numeric_t r_top = (((int) highsize) * xy_sum) - (x_sum * y_sum);
		numeric_t r_bot_x = sqrt((((int) highsize) * xx_sum) - (x_sum * x_sum));
		numeric_t r_bot_y = sqrt((((int) highsize) * yy_sum) - (y_sum * y_sum));
		numeric_t r = r_top / (r_bot_x * r_bot_y);
		return r;
	}

	linear_regression *find_linear_regression(numeric_array const &xdata, numeric_array const &ydata)
	{
		// uses ordinary least squares
		linear_regression *regression = new linear_regression;
		numeric_t r = correlation(xdata, ydata);
		numeric_t xm = mean(xdata);
		numeric_t ym = mean(ydata);
		numeric_t xs = stddev(xdata);
		numeric_t ys = stddev(ydata);
		regression->eq.slope = r * (ys / xs);
		regression->eq.yintercept = ym - regression->eq.slope * xm;
		numeric_t ss_res = residual_sum_squares(regression->eq, xdata, ydata);
		numeric_t ss_tot = total_sum_squares(ydata);
		regression->r2 = 1 - (ss_res / ss_tot);
		return regression;
	}

	numeric_t residual_sum_squares(linear_eq const &model, numeric_array const &xdata, numeric_array const &ydata)
	{
		numeric_t sm = 0;
		size_t highsize = (xdata.size() > ydata.size()) ? xdata.size() : ydata.size();
		for (size_t i = 0; i < highsize; i++) {
			numeric_t x, y;
			x = (xdata.size() < highsize) ? 0 : xdata[i];
			y = (ydata.size() < highsize) ? 0 : ydata[i];
			numeric_t res = y - (model.slope * x + model.yintercept);
			sm += pow(res, 2);
		}
		return sm;
	}

	numeric_t total_sum_squares(numeric_array const &data)
	{
		numeric_t avg = mean(data);
		numeric_t sm = 0;
		for (size_t i = 0; i < data.size(); i++) {
			numeric_t sq = pow(data[i] - avg, 2);
			sm += sq;
		}
		return sm;
	}
}

