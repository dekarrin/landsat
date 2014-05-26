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
}
#include <iostream>
namespace landsat
{

	template<typename T>
	class grid {

		private:
			T **data_store;
			size_t data_width;
			size_t data_height;
		public:
			grid(size_t width, size_t height) : data_store(new T*[height]), data_width(width), data_height(height)
			{
				for (size_t i = 0; i < data_height; i++) {
					data_store[i] = new T[width];
				}
			}

			virtual ~grid()
			{
				std::cout << "called " << std::endl;
				if (data_height > 0) {
					for (size_t i = data_height - 1; i != 0; i--) {
						std::cout << i << std::endl;
						delete data_store[i];
					}
					delete data_store[0];
				}
				delete data_store;
			}

			virtual void set(size_t x, size_t y, T value)
			{
				(*(data_store + y))[x] = value;
			}

			virtual T get(size_t x, size_t y) const
			{
				return (*(data_store + y))[x];
			}

			virtual size_t width() const
			{
				return data_width;
			}

			virtual size_t height() const
			{
				return data_height;
			}

			virtual T const *const *data() const
			{
				return data_store;
			}

			virtual T **data()
			{
				return data_store;
			}

	};

	template<typename T>
	class subgrid : public grid<T>
	{
		private:
			grid<T> *original_grid;
			size_t sub_width;
			size_t sub_height;
			size_t sub_x;
			size_t sub_y;

		public:
			subgrid(size_t x, size_t y, size_t width, size_t height, grid<T> *original) : grid<T>(0, 0), original_grid(original), sub_width(width), sub_height(height), sub_x(x), sub_y(y)
			{
			}

			virtual void set(size_t x, size_t y, T value)
			{
				original_grid->set(sub_x + x, sub_y + y, value);
			}

			virtual T get(size_t x, size_t y) const
			{
				return original_grid->get(sub_x + x, sub_y + y);
			}

			virtual size_t width() const
			{
				return sub_width;
			}

			virtual size_t height() const
			{
				return sub_height;
			}

			virtual size_t x() const
			{
				return sub_x;
			}

			virtual size_t y() const
			{
				return sub_y;
			}

			virtual T const *const *data() const
			{
				return data();
			}

			virtual T **data()
			{
				T **orig_row_start = (original_grid->data()) + sub_y;
				T **data_ptr;
				if (sub_x != 0) {
					data_ptr = new T*[height()];
					for (size_t i = 0; i < height(); i++) {
						*(data_ptr + i) = (*(orig_row_start + i)) + sub_x;
					}
				} else {
					data_ptr = orig_row_start;
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
