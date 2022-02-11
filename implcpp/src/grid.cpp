
#include "grid.hpp"
#include <cassert>
#include <algorithm>
#include <iostream>

namespace sart {

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

GridCoords::GridCoords(std::uint16_t x, std::uint16_t y):
	x(x), y(y)
{
	;
}

template<typename CellType, Mapping mapping>
Grid<CellType, mapping>::Grid(unsigned int side):
	side(side), data(new CellType[side * side])
{
	assert(is_power_of_two(this->side));
	assert(this->side <= (1 << 16));
}

template<typename CellType, Mapping mapping>
Grid<CellType, mapping>::Grid(Grid const& grid):
	side(grid.side), data(new CellType[grid.side * grid.side])
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
	assert(0 <= coords.x && coords.x < this->side);
	assert(0 <= coords.y && coords.y < this->side);

	std::uint32_t index = mapping(coords, this->side);
	//std::cout << "Access index " << index << " / " << this->side * this->side << std::endl;
	return this->data[index];
}

template class Grid<std::uint8_t>;

} /* sart */
