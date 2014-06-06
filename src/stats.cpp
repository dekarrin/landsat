#include "stats.hpp"
#include "sort.hpp"
#include <map>
#include <cstddef>
#include <cmath>

namespace landsat {

	static numeric_t stats_total_sum_squares(sequence<stats_t> const &data);
	static numeric_t numeric_total_sum_squares(
	 sequence<numeric_t> const &data);

	template <typename X, typename Y>
	static numeric_t correlation(sequence<X> const &xdata,
	 sequence<Y> const &ydata)
	{
		numeric_t x_sum, y_sum, xx_sum, yy_sum, xy_sum;
		x_sum = y_sum = xx_sum = yy_sum = xy_sum = 0;
		size_t highsize = xdata.size();
		if (xdata.size() < ydata.size()) {
			highsize = ydata.size();
		}
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
		numeric_t xy_prod = x_sum * y_sum;
		numeric_t xx_prod = x_sum * x_sum;
		numeric_t yy_prod = y_sum * y_sum;
		numeric_t r_top = (((numeric_t)highsize) * xy_sum) - xy_prod;
		numeric_t r_bot_x = sqrt((((int) highsize) * xx_sum) - xx_prod);
		numeric_t r_bot_y = sqrt((((int) highsize) * yy_sum) - yy_prod);
		numeric_t r = r_top / (r_bot_x * r_bot_y);
		return r;
	}
	
	template<typename X, typename Y>
	static bool check_horizontal_fit(linear_regression &reg,
	 sequence<X> const &xdata, sequence<Y> const &ydata)
	{
		bool is_horz = true;
		Y initial = ydata[0];
		for (size_t i = 1; i < ydata.size(); i++) {
			if (ydata[i] != initial) {
				is_horz = false;
				break;
			}
		}
		if (is_horz) {
			// if at least two data points are different,
			// we have a horizontal correlation
			bool x_is_same = true;
			X initialx = xdata[0];
			for (size_t i = 1; i < xdata.size(); i++) {
				if (xdata[i] != initialx) {
					x_is_same = false;
					break;
				}
			}
			if (!x_is_same) {
				reg.r2 = 1;
				reg.eq.slope = 0;
				reg.eq.intercept = ydata[0];
			} else {
				// the data collapses to one point;
				// impossible to regress
				reg.r2 = 0.0/0.0;
				reg.eq.slope = 0.0/0.0;
				reg.eq.intercept = 0.0/0.0;
			}
		}
		return is_horz;
	}

	template<typename X, typename Y>
	static numeric_t residual_sum_squares(linear_eq const &model,
	 sequence<X> const &xdata, sequence<Y> const &ydata)
	{
		numeric_t sm = 0;
		size_t highsize = xdata.size();
		if (xdata.size() < ydata.size()) {
			highsize = ydata.size();
		}
		for (size_t i = 0; i < highsize; i++) {
			X x;
			Y y;
			x = (xdata.size() < highsize) ? 0 : xdata[i];
			y = (ydata.size() < highsize) ? 0 : ydata[i];
			numeric_t res = y - (model.slope * x + model.intercept);
			sm += pow(res, 2);
		}
		return sm;
	}

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

	numeric_t numeric_min(sequence<numeric_t> const &data)
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

	stats_t stats_max(sequence<stats_t> const &data)
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

	numeric_t numeric_max(sequence<numeric_t> const &data)
	{
		data.reset();
		numeric_t mx = data.next();
		while (data.has_next()) {
			numeric_t n = data.next();
			if (mx < n) {
				mx = n;
			}
		}
		return mx;
	}

	numeric_t stats_median(sequence<stats_t> const &data)
	{
		data.reset();
		array<numeric_t> copy(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			copy[i] = data[i];
		}
		array<numeric_t> *sorted = sort_const(copy);
		numeric_t med = (*sorted)[sorted->size() / 2];
		if (sorted->size() % 2 == 0) {
			numeric_t med2 = (*sorted)[(sorted->size() / 2) - 1];
			med = ((med + med2) / 2);
		}
		delete sorted;
		return med;
	}

	numeric_t numeric_median(sequence<numeric_t> const &data)
	{
		data.reset();
		array<numeric_t> copy(data.size());
		for (size_t i = 0; i < data.size(); i++) {
			copy[i] = data[i];
		}
		array<numeric_t> *sorted = sort_const(copy);
		numeric_t med = (*sorted)[sorted->size() / 2];
		if (sorted->size() % 2 == 0) {
			numeric_t med2 = (*sorted)[(sorted->size() / 2) - 1];
			med = ((med + med2) / 2);
		}
		delete sorted;
		return med;
	}	

	stats_t stats_mode(sequence<stats_t> const &data)
	{
		data.reset();
		std::map<stats_t, int> counts;
		while (data.has_next()) {
			stats_t n = data.next();
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

	numeric_t numeric_mode(sequence<numeric_t> const &data)
	{
		data.reset();
		std::map<numeric_t, int> counts;
		while (data.has_next()) {
			numeric_t n = data.next();
			if (counts.count(n) == 0) {
				counts[n] = 0;
			}
			counts[n]++;
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

	numeric_t stats_mean(sequence<stats_t> const &data)
	{
		data.reset();
		numeric_t avg = 0;
		while (data.has_next()) {
			stats_t n = data.next();
			avg += n / static_cast<numeric_t>(data.size());
		}
		return avg;
	}

	numeric_t numeric_mean(sequence<numeric_t> const &data)
	{
		data.reset();
		numeric_t avg = 0;
		while (data.has_next()) {
			numeric_t n = data.next();
			avg += n / static_cast<numeric_t>(data.size());
		}
		return avg;
	}

	stats_t stats_range(sequence<stats_t> const &data)
	{
		stats_t mn = stats_min(data);
		stats_t mx = stats_max(data);
		stats_t rng = mx - mn;
		return rng;
	}

	numeric_t numeric_range(sequence<numeric_t> const &data)
	{
		numeric_t mn = numeric_min(data);
		numeric_t mx = numeric_max(data);
		numeric_t rng = mx - mn;
		return rng;
	}

	numeric_t stats_sum(sequence<stats_t> const &data)
	{
		data.reset();
		numeric_t sm = 0;
		while (data.has_next()) {
			sm += data.next();
		}
		return sm;
	}

	numeric_t numeric_sum(sequence<numeric_t> const &data)
	{
		data.reset();
		numeric_t sm = 0;
		while (data.has_next()) {
			sm += data.next();
		}
		return sm;
	}

	numeric_t stats_var(sequence<stats_t> const &data)
	{
		numeric_t avg = stats_mean(data);
		numeric_t varsum = 0;
		data.reset();
		while (data.has_next()) {
			varsum += pow(data.next() - avg, 2);
		}
		numeric_t v = varsum / (data.size() - 1);
		return v;
	}

	numeric_t numeric_var(sequence<numeric_t> const &data)
	{
		numeric_t avg = numeric_mean(data);
		numeric_t varsum = 0;
		data.reset();
		while (data.has_next()) {
			varsum += pow(data.next() - avg, 2);
		}
		numeric_t v = varsum / (data.size() - 1);
		return v;
	}

	numeric_t stats_var_pop(sequence<stats_t> const &data)
	{
		numeric_t avg = stats_mean(data);
		numeric_t varsum = 0;
		data.reset();
		while (data.has_next()) {
			varsum += pow(data.next() - avg, 2);
		}
		numeric_t v = varsum / data.size();
		return v;
	}

	numeric_t numeric_var_pop(sequence<numeric_t> const &data)
	{
		numeric_t avg = numeric_mean(data);
		numeric_t varsum = 0;
		data.reset();
		while (data.has_next()) {
			varsum += pow(data.next() - avg, 2);
		}
		numeric_t v = varsum / data.size();
		return v;
	}

	numeric_t stats_stddev(sequence<stats_t> const &data)
	{
		return sqrt(stats_var(data));
	}

	numeric_t numeric_stddev(sequence<numeric_t> const &data)
	{
		return sqrt(numeric_var(data));
	}

	numeric_t stats_stddev_pop(sequence<stats_t> const &data)
	{
		return sqrt(stats_var_pop(data));
	}

	numeric_t numeric_stddev_pop(sequence<numeric_t> const &data)
	{
		return sqrt(numeric_var_pop(data));
	}

	numeric_t stats_correlation(sequence<stats_t> const &xdata,
	 sequence<stats_t> const &ydata)
	{
		return correlation(xdata, ydata);
	}

	numeric_t numeric_correlation(sequence<numeric_t> const &xdata,
	 sequence<numeric_t> const &ydata)
	{
		return correlation(xdata, ydata);
	}

	numeric_t stats_numeric_correlation(sequence<stats_t> const &xdata,
	 sequence<numeric_t> const &ydata)
	{
		return correlation(xdata, ydata);
	}

	numeric_t numeric_stats_correlation(sequence<numeric_t> const &xdata,
	 sequence<stats_t> const &ydata)
	{
		return correlation(xdata, ydata);
	}

	linear_regression *stats_find_linear_regression(
	 sequence<stats_t> const &xdata, sequence<stats_t> const &ydata)
	{
		// uses ordinary least squares
			
		linear_regression *regression = new linear_regression;
		// first test the special case of a horizontal fit:
		if (!check_horizontal_fit(*regression, xdata, ydata)) {
			numeric_t r = stats_correlation(xdata, ydata);
			numeric_t xm = stats_mean(xdata);
			numeric_t ym = stats_mean(ydata);
			numeric_t xs = stats_stddev(xdata);
			numeric_t ys = stats_stddev(ydata);
			linear_eq *eq = &(regression->eq);
			eq->slope = r * (ys / xs);
			eq->intercept = ym - eq->slope * xm;
			numeric_t ss_res =
			 residual_sum_squares(*eq, xdata, ydata);
			numeric_t ss_tot = stats_total_sum_squares(ydata);
			regression->r2 = 1 - (ss_res / ss_tot);
		}
		return regression;
	}

	linear_regression *numeric_find_linear_regression(
	 sequence<numeric_t> const &xdata, sequence<numeric_t> const &ydata)
	{
		// uses ordinary least squares
			
		linear_regression *regression = new linear_regression;
		// first test the special case of a horizontal fit:
		if (!check_horizontal_fit(*regression, xdata, ydata)) {
			numeric_t r = numeric_correlation(xdata, ydata);
			numeric_t xm = numeric_mean(xdata);
			numeric_t ym = numeric_mean(ydata);
			numeric_t xs = numeric_stddev(xdata);
			numeric_t ys = numeric_stddev(ydata);
			linear_eq *eq = &(regression->eq);
			eq->slope = r * (ys / xs);
			eq->intercept = ym - eq->slope * xm;
			numeric_t ss_res =
			 residual_sum_squares(*eq, xdata, ydata);
			numeric_t ss_tot = numeric_total_sum_squares(ydata);
			regression->r2 = 1 - (ss_res / ss_tot);
		}
		return regression;
	}

	linear_regression *stats_numeric_find_linear_regression(
	 sequence<stats_t> const &xdata, sequence<numeric_t> const &ydata)
	{
		// uses ordinary least squares
			
		linear_regression *regression = new linear_regression;
		// first test the special case of a horizontal fit:
		if (!check_horizontal_fit(*regression, xdata, ydata)) {
			numeric_t r = stats_numeric_correlation(xdata, ydata);
			numeric_t xm = stats_mean(xdata);
			numeric_t ym = numeric_mean(ydata);
			numeric_t xs = stats_stddev(xdata);
			numeric_t ys = numeric_stddev(ydata);
			linear_eq *eq = &(regression->eq);
			eq->slope = r * (ys / xs);
			eq->intercept = ym - eq->slope * xm;
			numeric_t ss_res =
			 residual_sum_squares(*eq, xdata, ydata);
			numeric_t ss_tot = numeric_total_sum_squares(ydata);
			regression->r2 = 1 - (ss_res / ss_tot);
		}
		return regression;
	}

	linear_regression *numeric_stats_find_linear_regression(
	 sequence<numeric_t> const &xdata, sequence<stats_t> const &ydata)
	{
		// uses ordinary least squares
			
		linear_regression *regression = new linear_regression;
		// first test the special case of a horizontal fit:
		if (!check_horizontal_fit(*regression, xdata, ydata)) {
			numeric_t r = numeric_stats_correlation(xdata, ydata);
			numeric_t xm = numeric_mean(xdata);
			numeric_t ym = stats_mean(ydata);
			numeric_t xs = numeric_stddev(xdata);
			numeric_t ys = stats_stddev(ydata);
			linear_eq *eq = &(regression->eq);
			eq->slope = r * (ys / xs);
			eq->intercept = ym - eq->slope * xm;
			numeric_t ss_res =
			 residual_sum_squares(*eq, xdata, ydata);
			numeric_t ss_tot = stats_total_sum_squares(ydata);
			regression->r2 = 1 - (ss_res / ss_tot);
		}
		return regression;
	}

	static numeric_t stats_total_sum_squares(sequence<stats_t> const &data)
	{
		numeric_t avg = stats_mean(data);
		numeric_t sm = 0;
		for (size_t i = 0; i < data.size(); i++) {
			numeric_t sq = pow(data[i] - avg, 2);
			sm += sq;
		}
		return sm;
	}

	static numeric_t numeric_total_sum_squares(
	 sequence<numeric_t> const &data)
	{
		numeric_t avg = numeric_mean(data);
		numeric_t sm = 0;
		for (size_t i = 0; i < data.size(); i++) {
			numeric_t sq = pow(data[i] - avg, 2);
			sm += sq;
		}
		return sm;
	}
}

