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
	struct grid
	{
		public:
			T *data;
			size_t width;
			size_t height;
			bool is_sub;
			grid(size_t width, size_t height) : data(new T[width * height]), width(width), height(height), is_sub(false)
			{}

			grid(grid<T> *old_grid, rect const &sub) : data(old_grid->data + (old_grid->height * sub.y) + sub.x), width(sub.width), height(sub.height), is_sub(true)
			{}

			~grid()
			{
				if (!is_sub) {
					delete data;
				}
			}
	};

}
