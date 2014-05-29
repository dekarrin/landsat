#pragma once

#include <cstddef>

namespace landsat
{
	template<typename T>
	class array
	{
		private:
			T *data_store;
			size_t data_size;

		public:
			array(size_t size) : data_store(new T[size]), data_size(size)
			{}

			array(array const &a2) : data_store(new T[a2.size()]), data_size(a2.size())
			{
				for (size_t i = 0; i < a2.size(); i++) {
					data_store[i] = a2[i];
				}
			}

			array(T const *ptr, size_t size) : data_store(new T[size]), data_size(size)
			{
				for (size_t i = 0; i < size; i++) {
					data_store[i] = ptr[i];
				}
			}

			~array()
			{
				delete[] data_store;
			}

			array &operator=(array const &a2)
			{
				delete[] data_store;
				data_size = a2.size();
				data_store = new T[a2.size()];
				for (size_t i = 0; i < a2.size; i++) {
					data_store[i] = a2[i];
				}
			}

			T &operator[](size_t index)
			{
				return data_store[index];
			}

			T const &operator[](size_t index) const
			{
				return data_store[index];
			}

			T *data()
			{
				return data_store;
			}

			T const *data() const
			{
				return data_store;
			}

			size_t size() const
			{
				return data_size;
			}

			size_t element_size() const
			{
				return sizeof(T);
			}

			void swap(size_t p1, size_t p2)
			{
				T temp = data_store[p1];
				data_store[p1] = data_store[p2];
				data_store[p2] = temp;
			}
	};
	
	template <typename T>
	struct grid
	{
		public:
			T *data;
			size_t width;
			size_t height;
			bool is_sub;
			grid(size_t width, size_t height) : data(new T[width * height]), width(width), height(height), is_sub(false)
			{}

			grid(grid<T> *old_grid, size_t x, size_t y, size_t width, size_t height) : data(old_grid->data + (old_grid->height * y) + x), width(width), height(height), is_sub(true)
			{}

			~grid()
			{
				if (!is_sub) {
					delete data;
				}
			}
	};

	typedef double numeric_t;
	typedef array<numeric_t> numeric_array;
	typedef unsigned short pixel_t;

	struct linear_eq
	{
		numeric_t yintercept;
		numeric_t slope;
	};

	struct linear_regression
	{
		linear_eq eq;
		numeric_t r2;
	};

}