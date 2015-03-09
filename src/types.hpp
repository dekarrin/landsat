#pragma once

#include <cstddef>
#include <iostream>
#include <cstdint>

namespace landsat
{
	template <typename T>
	class rect
	{
		public:
			T x;
			T y;
			T width;
			T height;
	};

	template <typename T>
	class sequence
	{
		public:
			virtual ~sequence() {}
			void reset() const
			{
				reset(1, 1);
			}

			virtual void reset(size_t step, int dir) const = 0;
			virtual T next() const = 0;
			virtual bool has_next() const = 0;
			virtual T const &operator[](size_t index) const = 0;
			virtual T &operator[](size_t index) = 0;
			virtual size_t size() const = 0;
	};

	template <typename T>
	class array : public sequence<T>
	{
		private:
			T *data_store;
			size_t data_size;
			mutable size_t iterator;
			mutable size_t iter_step;
			mutable int iter_dir;

		public:
			array(size_t size) : data_store(new T[size]),
			 data_size(size), iterator(0), iter_step(1), iter_dir(1)
			{}

			array(array const &a2) : data_store(new T[a2.size()]),
			 data_size(a2.size()), iterator(0), iter_step(1),
			 iter_dir(1)
			{
				for (size_t i = 0; i < a2.size(); i++) {
					data_store[i] = a2[i];
				}
			}

			array(T const *ptr, size_t size) :
			 data_store(new T[size]), data_size(size), iterator(0),
			 iter_step(1), iter_dir(1)
			{
				for (size_t i = 0; i < size; i++) {
					data_store[i] = ptr[i];
				}
			}

			virtual ~array()
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

			virtual T &operator[](size_t index)
			{
				if (index < data_size && index >= 0) {
					return data_store[index];
				} else {
					// deliberate segfault if illegal access
					return *(static_cast<T*>(NULL));
				}
			}

			virtual T const &operator[](size_t index) const
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

			virtual size_t size() const
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

			virtual void reset(size_t step, int dir) const
			{
				iter_step = step;
				iter_dir = dir;
				if (iter_dir < 0) {
					iterator = data_size - 1;
				} else {
					iterator = 0;
				}
			}

			virtual T next() const
			{
				T val = data_store[iterator];
				if (iter_dir >= 0) {
					iterator += iter_step;
				}
				if (iter_dir < 0) {
					iterator -= iter_step;
				}
				return val;
			}
			
			virtual bool has_next() const
			{
				if (iter_dir < 0) {
					return iterator >= iter_step;
				} else {
					return iterator < data_size;
				}
			}
	};
	
	template <typename T>
	class grid : public sequence<T>
	{
		private:
			T **data_store;
			size_t data_width;
			size_t data_height;
			bool shares_row_stores;
			bool shares_data_store;
			mutable size_t iterator_x;
			mutable size_t iterator_y;
			mutable size_t iter_step;
			mutable int iter_dir;

		public:
			grid(size_t width, size_t height) :
			 data_store(new T*[height]), data_width(width),
			 data_height(height), shares_row_stores(false),
			 shares_data_store(false), iterator_x(0), iterator_y(0),
			 iter_step(1), iter_dir(1)
			{
				for (size_t i = 0; i < height; i++) {
					data_store[i] = new T[width];
				}
			}

			grid(grid<T> *old_grid, rect<size_t> const &sub) :
			 data_store(old_grid->data_store + sub.y),
			 data_width(sub.width), data_height(sub.height),
			 shares_row_stores(true), shares_data_store(true),
			 iterator_x(0), iterator_y(0), iter_step(1), iter_dir(1)
			{
				if (sub.x != 0) {
					data_store = new T*[sub.height];
					shares_data_store = false;
					for (size_t i = 0; i < data_height; i++)
					 {
						data_store[i] =
						 (*(old_grid->data_store +
						 sub.y + i)) + sub.x;
					}
				}
			}

			virtual ~grid()
			{
				if (!shares_data_store) {
					if (!shares_row_stores) {
						for (size_t i = 0;
						 i < data_height; i++) {
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

			virtual void reset(size_t step, int dir) const
			{
				iter_step = step;
				iter_dir = dir;
				iterator_x = 0;
				iterator_y = (iter_dir < 0) ? data_height : 0;
			}

			virtual T next() const
			{	
				if (iter_dir < 0) {
					for (size_t i = 0; i < iter_step; i++) {
						if (iterator_x == 0) {
							iterator_y--;
							iterator_x = data_width;
						}
						iterator_x--;
					}
				}
				T val = get(iterator_x, iterator_y);
				if (iter_dir >= 0) {
					for (size_t i = 0; i < iter_step; i++) {
						iterator_x++;
						if (iterator_x == data_width) {
							iterator_x = 0;
							iterator_y++;
						}
					}
				}
				return val;
			}

			virtual bool has_next() const
			{
				if (iter_dir < 0) {
					return ((iterator_y * data_width) +
					 iterator_x) >= iter_step;
				} else {
					return iterator_y < data_height;
				}
			}

			virtual T const &operator[](size_t index) const
			{
				size_t y = index / data_width;
				size_t x = index % data_width;
				return (data_store[y])[x];
			}

			virtual T &operator[](size_t index)
			{
				size_t y = index / data_width;
				size_t x = index % data_width;
				return (data_store[y])[x];
			}

			size_t size() const
			{
				return data_width * data_height;
			}
	};

	typedef union {
		uint8_t d8;
		uint16_t d16;
		uint32_t d32;
		uint64_t d64;
	} pixel_t;

	typedef {
		grid<pixel_t> *grid;
		unsigned int size; 
	} pixel_grid;
}
