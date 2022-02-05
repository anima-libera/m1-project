
#include "grid.hpp"
#include <cassert>
#include <algorithm>

namespace sart {

inline static bool is_power_of_two(unsigned int x)
{
	return (x & (x - 1)) == 0 && x != 0;
}

inline static std::uint32_t row_mapping(CoordsHD coords, std::size_t side)
{
	return static_cast<std::uint32_t>(coords.x) + side * static_cast<std::uint32_t>(coords.y);
}

/* Trick from http://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
 * that can be understood by looking at https://en.wikipedia.org/wiki/File:Z-curve.svg */
inline static std::uint32_t zorder_mapping(CoordsHD coords, std::size_t side)
{
	(void)side;

	constexpr std::uint32_t spreading_patterns[] = {
		0b01010101010101010101010101010101,
		0b00110011001100110011001100110011,
		0b00001111000011110000111100001111,
		0b00000000111111110000000011111111};
	auto spread_bits = [](std::uint32_t& x)
	{
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

std::uint32_t CoordsHD::to_index(std::size_t side)
{
	//return row_mapping(*this, side);
	return zorder_mapping(*this, side);
}

template<typename T>
Grid<T>::Grid(std::size_t side):
	side(side), data(new T[side])
{
	assert(is_power_of_two(this->side));
	assert(this->side <= (1 << 16));
}

template<typename T>
Grid<T>::Grid(Grid const& grid):
	side(grid.side), data(new T[grid.side])
{
	std::copy(grid.data, grid.data + grid.side * grid.side, this->data);
	/* Elements of this->data are constructed twice, aren't they?
	 * But if data is initialized with allocated uninitailzed "raw" memory,
	 * then does std::copy destruct garbage T objects from there before constructing copies ?
	 * TODO: Find out and optimize. */
}

template<typename T>
Grid<T>::~Grid()
{
	delete[] this->data;
}

template<typename T>
T& Grid<T>::access(CoordsHD coords)
{
	assert(0 <= coords.x && coords.x < this->side);
	assert(0 <= coords.y && coords.y < this->side);

	return this->data[coords.to_index(this->side)];
}

template class Grid<float>;

} /* sart */
