#pragma once

#include <cstddef>
#include <iostream>

namespace landsat
{
	
	struct rect
	{
		int x;
		int y;
		int width;
		int height;
	};

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
				if (index < data_size && index >= 0) {
					return data_store[index];
				} else {
					// deliberate segfault if illegal access
					return *(static_cast<T*>(NULL));
				}
			}

			T const &operator[](size_t index) const
			{
				if (index < data_size && index >= 0) {
					return data_store[index];
				} else {
					// deliberate segfault if illegal access
					return *(static_cast<T*>(NULL));
				}
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
	class grid
	{
		private:
			T **data_store;
			size_t data_width;
			size_t data_height;
			bool shares_row_stores;
			bool shares_data_store;

		public:
			grid(size_t width, size_t height) : data_store(new T*[height]), data_width(width), data_height(height), shares_row_stores(false), shares_data_store(false)
			{
				for (size_t i = 0; i < height; i++) {
					data_store[i] = new T[width];
				}
			}

			grid(grid<T> *old_grid, rect const &sub) : data_store(old_grid->data_store + sub.y), data_width(sub.width), data_height(sub.height), shares_row_stores(true), shares_data_store(true)
			{
				if (sub.x != 0) {
					data_store = new T*[sub.height];
					shares_data_store = false;
					for (size_t i = 0; i < data_height; i++) {
						data_store[i] = (*(old_grid->data_store + sub.y + i)) + sub.x;
					}
				}
			}

			~grid()
			{
				if (!shares_data_store) {
					if (!shares_row_stores) {
						for (size_t i = 0; i < data_height; i++) {
							delete[] data_store[i];
						}
					}
					delete[] data_store;
				}
			}

			T *row(size_t y)
			{
				return data_store[y];
			}

			T const *row(size_t y) const
			{
				return data_store[y];
			}

			T get(size_t x, size_t y) const
			{
				return (data_store[y])[x];
			}

			void set(size_t x, size_t y, T value)
			{
				(data_store[y])[x] = value;
			}

			size_t width() const
			{
				return data_width;
			}

			size_t height() const
			{
				return data_height;
			}

			bool is_sub() const
			{
				return shares_row_stores;
			}
	};

}
