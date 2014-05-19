#pragma once

#include <cstddef>

namespace stats {

	template <typename T>
	T min(T const * data, size_t size);

	template <typename T>
	T max(T const * data, size_t size);

	template <typename T>
	T median(T const * data, size_t size);

	template <typename T>
	T mode(T const * data, size_t size);

	template <typename T>
	T mean(T const * data, size_t size);

	template <typename T>
	T mean_geometric(T const * data, size_t size);

	template <typename T>
	T mean_harmonic(T const * data, size_t size);

	template <typename T>
	T range(T const * data, size_t size);

}
