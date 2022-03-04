
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

std::uint32_t MappingRow::coords_to_index(GridCoords coords, unsigned int side)
{
	return static_cast<std::uint32_t>(coords.x) + side * static_cast<std::uint32_t>(coords.y);
}

GridCoords MappingRow::index_to_coords(std::uint32_t index, unsigned int side)
{
	return GridCoords{
		static_cast<std::uint16_t>(index % side),
		static_cast<std::uint16_t>(index / side)};
}

std::uint32_t MappingZorder::coords_to_index(GridCoords coords, [[maybe_unused]] unsigned int side)
{
	/* Trick from http://graphics.stanford.edu/~seander/bithacks.html#InterleaveBMN
	* that can be understood by looking at https://en.wikipedia.org/wiki/File:Z-curve.svg */
	auto const spread_bits = [](std::uint32_t& x)
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

#if 0
GridCoords::GridCoords(std::uint16_t x, std::uint16_t y):
	x{x}, y{y}
{
	;
}
#endif

GridCoords::GridCoords(unsigned int x, unsigned int y):
	x{static_cast<std::uint16_t>(x)}, y{static_cast<std::uint16_t>(y)}
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

RawRect::RawRect(unsigned int side):
	side{side}
{
	;
}

RawRect::Iterator::Iterator():
	is_past_the_end{true}
{
	;
}

RawRect::Iterator::Iterator(unsigned int side):
	/* Mid-point line drawing variables initialization. */
	head{0, 0},
	side{side},
	is_past_the_end{false}
{
	;
}

bool RawRect::Iterator::operator==(Iterator const& right) const
{
	/* This should only be called by ranged-based for loop internals hopefully...
	 * Wh- what have I done?! */
	return this->is_past_the_end && right.is_past_the_end;
}

bool RawRect::Iterator::operator!=(Iterator const& right) const
{
	return not (*this == right);
}

RawRect::Iterator& RawRect::Iterator::operator++()
{
	this->head.x++;
	if (this->head.x >= this->side)
	{
		this->head.x = 0;
		this->head.y++;
		if (this->head.y >= this->side)
		{
			this->is_past_the_end = true;
		}
	}
	return *this;
}

RawRect::Iterator RawRect::Iterator::operator++(int)
{
	Iterator pre_increment_state = *this;
	++(*this);
	return pre_increment_state;
}

GridCoords RawRect::Iterator::operator*() const
{
	return this->head;
}

RawRect::Iterator RawRect::begin() const
{
	return Iterator{this->side};
}

RawRect::Iterator RawRect::end() const
{
	return Iterator{};
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
	/* If the count (of string overlapping the pixel) is 0, then it means we can see
	 * the "background color" through this pixel and it should appear white.
	 * Else if the count is non-zero, then it means at least one string covers this pixel
	 * and it should appear the color of the strings, i.e. black. */
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


template<typename CellType, typename Mapping>
Grid<CellType, Mapping>::Grid(unsigned int side):
	side{side}, data{new CellType[side * side]}
{
	if constexpr (std::is_same_v<Mapping, MappingRow>)
	{
		assert(is_power_of_two(this->side));
		assert(this->side <= GridCoords::coord_max_value);
	}
}

template<typename CellType, typename Mapping>
Grid<CellType, Mapping>::Grid(unsigned int side, std::function<CellType(GridCoords)> init_function):
	Grid{side}
{
	for (GridCoords coords : RawRect{side})
	{
		this->access(coords) = init_function(coords);
	}
}

template<typename CellType, typename Mapping>
Grid<CellType, Mapping>::Grid(Grid const& grid):
	side{grid.side}, data{new CellType[grid.side * grid.side]}
{
	std::copy(grid.data, grid.data + grid.side * grid.side, this->data);
	/* Elements of this->data are constructed twice, aren't they?
	 * But if data is initialized with allocated uninitailzed "raw" memory,
	 * then does std::copy destruct garbage T objects from there before constructing copies ?
	 * TODO: Find out and optimize. */
}

template<typename CellType, typename Mapping>
Grid<CellType, Mapping>::~Grid()
{
	delete[] this->data;
}

template<typename CellType, typename Mapping>
CellType& Grid<CellType, Mapping>::access(GridCoords coords)
{
	if ((not (0 <= coords.x && coords.x < this->side)) ||
		(not (0 <= coords.y && coords.y < this->side)))
	{
		//std::cout << "Warning: Access at coords (" << coords.x << ", " << coords.y << ") "
		//	<< "on a grid of size " << this->side << "x" << this->side << std::endl;
		//return this->data[0];
		assert(false);
	}

	std::uint32_t const index = Mapping::coords_to_index(coords, this->side);
	return this->data[index];
}

template<typename CellType, typename Mapping>
CellType const& Grid<CellType, Mapping>::access(GridCoords coords) const
{
	if ((not (0 <= coords.x && coords.x < this->side)) ||
		(not (0 <= coords.y && coords.y < this->side)))
	{
		//std::cout << "Warning: Access at coords (" << coords.x << ", " << coords.y << ") "
		//	<< "on a grid of size " << this->side << "x" << this->side << std::endl;
		//return this->data[0];
		assert(false);
	}

	std::uint32_t const index = Mapping::coords_to_index(coords, this->side);
	return this->data[index];
}

/* TODO: Remove code duplication between the two Grid::access definitions. */

template<typename CellType, typename Mapping>
void const* Grid<CellType, Mapping>::raw_data() const
{
	return &this->data[0];
}

template<typename CellType, typename Mapping>
void Grid<CellType, Mapping>::output_as_bitmap(char const* output_file_path) const
{
	using BitmapPixel = PixelRgba<std::uint8_t>;

	if constexpr (std::is_same_v<CellType, BitmapPixel> && std::is_same_v<Mapping, MappingRow>)
	{
		output_bitmap(*this, output_file_path);
	}
	else
	{
		/* If the grid is not bitmap compatible,
		 * then convert it to a local bitmap compatible copy. */
		BitmapComatibleGrid bitmap = convert_grid<CellType, Mapping, BitmapPixel, MappingRow>(
			*this,
			static_cast<std::function<BitmapPixel(CellType)>>(
				[](CellType pixel){return static_cast<PixelRgba<std::uint8_t>>(pixel);})
		);
		
		/* This method call should not be recursive.
		 * If execution ends up here again in the nested call,
		 * then it will recursively call itself in an infinite loop
		 * that will fill up the heap and the stack... */
		bitmap.output_as_bitmap(output_file_path);
	}
}

template class Grid<PixelGs<std::uint8_t>, MappingRow>;
template class Grid<PixelCount<std::uint8_t>, MappingRow>;
template class Grid<PixelCount<std::uint16_t>, MappingRow>;
template class Grid<PixelRgba<std::uint8_t>, MappingRow>;
template class Grid<PixelGs<std::uint8_t>, MappingZorder>;
template class Grid<PixelCount<std::uint8_t>, MappingZorder>;
template class Grid<PixelCount<std::uint16_t>, MappingZorder>;
template class Grid<PixelRgba<std::uint8_t>, MappingZorder>;

template<typename CellTypeSrc, typename MappingSrc, typename CellTypeDst, typename MappingDst>
Grid<CellTypeDst, MappingDst> convert_grid(
	Grid<CellTypeSrc, MappingSrc> const& grid_src,
	std::function<CellTypeDst(CellTypeSrc)> convert_cell)
{
	Grid<CellTypeDst, MappingDst> grid_dst{grid_src.side};
	for (GridCoords coords : RawRect{grid_src.side})
	{
		grid_dst.access(coords) = convert_cell(grid_src.access(coords));
	}
	return grid_dst;
}

template<typename CellType, typename MappingSrc, typename MappingDst>
Grid<CellType, MappingDst> convert_grid_mapping(Grid<CellType, MappingSrc> const& grid_src)
{
	Grid<CellType, MappingDst> grid_dst{grid_src.side};
	for (GridCoords coords : RawRect{grid_src.side})
	{
		grid_dst.access(coords) = grid_src.access(coords);
	}
	return grid_dst;
}

template Grid<PixelGs<std::uint8_t>, MappingZorder> convert_grid_mapping(
	Grid<PixelGs<std::uint8_t>, MappingRow> const& grid_src);

template Grid<PixelGs<std::uint8_t>, MappingRow> convert_grid_mapping(
	Grid<PixelGs<std::uint8_t>, MappingRow> const& grid_src);

} /* StringArtRennes */
