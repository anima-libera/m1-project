
#include "grid.hpp"
#include "bitmap.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>

namespace StringArtRennes
{

static inline bool is_power_of_two(unsigned int x)
{
	return (x & (x - 1)) == 0 && x != 0;
}

std::uint32_t row_mapping(GridCoords coords, unsigned int side)
{
	return static_cast<std::uint32_t>(coords.x) + side * static_cast<std::uint32_t>(coords.y);
}

GridCoords row_mapping_inverse(std::uint32_t index, unsigned int side)
{
	return GridCoords{
		static_cast<std::uint16_t>(index % side),
		static_cast<std::uint16_t>(index / side)};
}

/* Trick from http://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
 * that can be understood by looking at https://en.wikipedia.org/wiki/File:Z-curve.svg */
std::uint32_t zorder_mapping(GridCoords coords, unsigned int side)
{
	(void)side;

	auto spread_bits = [](std::uint32_t& x)
	{
		constexpr std::uint32_t spreading_patterns[] = {
			0b01010101010101010101010101010101,
			0b00110011001100110011001100110011,
			0b00001111000011110000111100001111,
			0b00000000111111110000000011111111};

		x = (x | (x << 8)) & spreading_patterns[3];
		x = (x | (x << 4)) & spreading_patterns[2];
		x = (x | (x << 2)) & spreading_patterns[1];
		x = (x | (x << 1)) & spreading_patterns[0];
	};

	std::uint32_t spreaded_x = coords.x;
	spread_bits(spreaded_x);
	std::uint32_t spreaded_y = coords.y;
	spread_bits(spreaded_y);
	return spreaded_x | (spreaded_y << 1);
}


GridCoords::GridCoords():
	x{0}, y{0}
{
	;
}

GridCoords::GridCoords(std::uint16_t x, std::uint16_t y):
	x{x}, y{y}
{
	;
}

bool GridCoords::operator==(GridCoords const& right) const
{
	return this->x == right.x && this->y == right.y;
}

bool GridCoords::operator!=(GridCoords const& right) const
{
	return not (*this == right);
}


template<typename T>
PixelGs<T>::PixelGs()
{
	;
}

template<typename T>
PixelGs<T>::PixelGs(T gs):
	gs{gs}
{
	;
}

template<typename T>
PixelGs<T>::operator PixelRgba<std::uint8_t>() const
{
	return PixelRgba<std::uint8_t>{this->gs, this->gs, this->gs, 255};
}

template class PixelGs<std::uint8_t>;

template<typename T>
PixelCount<T>::PixelCount():
	count{0}
{
	;
}

template<typename T>
PixelCount<T>::PixelCount(T count):
	count{count}
{
	;
}

template<typename T>
PixelCount<T>::operator PixelRgba<std::uint8_t>() const
{
	std::uint8_t const gs = this->count == 0 ? 255 : 0;
	return PixelRgba<std::uint8_t>{gs, gs, gs, 255};
}

template class PixelCount<std::uint8_t>;
template class PixelCount<std::uint16_t>;

template<typename T>
PixelRgba<T>::PixelRgba()
{
	;
}

template<typename T>
PixelRgba<T>::PixelRgba(T r, T g, T b, T a):
	rgba{r, g, b, a}
{
	;
}

template class PixelRgba<std::uint8_t>;


template<typename CellType, Mapping mapping>
Grid<CellType, mapping>::Grid(unsigned int side):
	side(side), data(new CellType[side * side])
{
	assert(is_power_of_two(this->side));
	assert(this->side <= GridCoords::coord_max_value);
}

template<typename CellType, Mapping mapping>
Grid<CellType, mapping>::Grid(Grid const& grid):
	side{grid.side}, data{new CellType[grid.side * grid.side]}
{
	std::copy(grid.data, grid.data + grid.side * grid.side, this->data);
	/* Elements of this->data are constructed twice, aren't they?
	 * But if data is initialized with allocated uninitailzed "raw" memory,
	 * then does std::copy destruct garbage T objects from there before constructing copies ?
	 * TODO: Find out and optimize. */
}

template<typename CellType, Mapping mapping>
Grid<CellType, mapping>::~Grid()
{
	delete[] this->data;
}

template<typename CellType, Mapping mapping>
CellType& Grid<CellType, mapping>::access(GridCoords coords)
{
	if ((not (0 <= coords.x && coords.x < this->side)) ||
		(not (0 <= coords.y && coords.y < this->side)))
	{
		//std::cout << "Warning: Access at coords (" << coords.x << ", " << coords.y << ") "
		//	<< "on a grid of size " << this->side << "x" << this->side << std::endl;
		//return this->data[0];
		assert(false);
	}

	std::uint32_t const index = mapping(coords, this->side);
	return this->data[index];
}

template<typename CellType, Mapping mapping>
CellType const& Grid<CellType, mapping>::access(GridCoords coords) const
{
	if ((not (0 <= coords.x && coords.x < this->side)) ||
		(not (0 <= coords.y && coords.y < this->side)))
	{
		//std::cout << "Warning: Access at coords (" << coords.x << ", " << coords.y << ") "
		//	<< "on a grid of size " << this->side << "x" << this->side << std::endl;
		//return this->data[0];
		assert(false);
	}

	std::uint32_t const index = mapping(coords, this->side);
	return this->data[index];
}

/* TODO: Remove code duplication between the two Grid::access definitions. */

/* TODO: Better handling of out-of-bounds access. */

template<typename CellType, Mapping mapping>
void const* Grid<CellType, mapping>::raw_data() const
{
	return &this->data[0];
}

template<typename CellType, Mapping mapping>
void Grid<CellType, mapping>::output_as_bitmap(char const* output_file_path) const
{
	//if constexpr (std::is_same_v<decltype(*this), BitmapComatibleGrid>)
	/* Note: For some reason the above condition is not equivalent to the following one. */
	if constexpr (std::is_same_v<CellType, PixelRgba<std::uint8_t>> && mapping == row_mapping)
	{
		output_bitmap(*this, output_file_path);
	}
	else
	{
		/* If the grid is not bitmap compatible,
		 * then convert it to a local bitmap compatible copy. */
		BitmapComatibleGrid bitmap{this->side};
		for (unsigned int y = 0; y < this->side; y++)
		for (unsigned int x = 0; x < this->side; x++)
		{
			bitmap.access(GridCoords{x, y}) =
				static_cast<PixelRgba<std::uint8_t>>(this->access(GridCoords{x, y}));
		}
		
		/* This method call should not be recursive.
		 * If execution ends up here again in the nested call,
		 * then it will recursively call itself in an infinite loop
		 * that will fill up the heap and the stack... */
		bitmap.output_as_bitmap(output_file_path);
	}
}

template class Grid<PixelGs<std::uint8_t>, row_mapping>;
template class Grid<PixelCount<std::uint8_t>, row_mapping>;
template class Grid<PixelCount<std::uint16_t>, row_mapping>;
template class Grid<PixelRgba<std::uint8_t>, row_mapping>;

} /* StringArtRennes */
