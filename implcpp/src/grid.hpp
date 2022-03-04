
#ifndef HEADER_GRID_
#define HEADER_GRID_

#include <cstdint>
#include <array>
#include <type_traits>
#include <functional>

namespace StringArtRennes
{

/* Coordinates of a cell in a grid. */
class GridCoords
{
public:
	static constexpr unsigned int coord_max_value = UINT16_MAX;

public:
	std::uint16_t x;
	std::uint16_t y;

public:
	GridCoords();
	#if 0
	GridCoords(std::uint16_t x, std::uint16_t y);
	#endif
	GridCoords(unsigned int x, unsigned int y);
	bool operator==(GridCoords const& right) const;
	bool operator!=(GridCoords const& right) const;
};

class RawRect
{
public:
	unsigned int side;

public:
	RawRect(unsigned int side);

public:
	class Iterator
	{
	public:
		GridCoords head;
		unsigned int side;
		bool is_past_the_end; /* True iff equal to the past-the-end iterator. */
	public:
		Iterator(); /* Past-the-end. */
		Iterator(unsigned int side); /* Not past-the-end. */
		bool operator==(Iterator const& right) const;
		bool operator!=(Iterator const& right) const;
		Iterator& operator++();
		Iterator operator++(int);
		GridCoords operator*() const;
	};

public:
	Iterator begin() const;
	Iterator end() const;
};

#if 0
/* Mapping of the cells of a 2D square grid in 1D memory representation. */
using Mapping = std::uint32_t (*)(GridCoords coords, unsigned int side);

/* Row mapping, the classic. */
std::uint32_t row_mapping(GridCoords coords, unsigned int side);
GridCoords row_mapping_inverse(std::uint32_t index, unsigned int side);

/* Z-Order mapping, see implementation for details. */
std::uint32_t zorder_mapping(GridCoords coords, unsigned int side);
#endif

class MappingRow
{
public:
	static std::uint32_t coords_to_index(GridCoords coords, unsigned int side);
	static GridCoords index_to_coords(std::uint32_t index, unsigned int side);
};

class MappingZorder
{
public:
	static std::uint32_t coords_to_index(GridCoords coords, unsigned int side);
	static GridCoords index_to_coords(std::uint32_t index, unsigned int side);
};


template<typename T>
class PixelRgba;

template<typename T>
class PixelGs
{
	static_assert(std::is_arithmetic_v<T>);

public:
	T gs; /* Grayscale value, 0 is black, max is white. */

public:
	PixelGs();
	PixelGs(T gs);
	explicit operator PixelRgba<std::uint8_t>() const;
};

template<typename T>
class PixelCount
{
	static_assert(std::is_integral_v<T>);

public:
	T count; /* Count of the number of strings overlaping the pixel. */

public:
	PixelCount();
	PixelCount(T count);
	explicit operator PixelRgba<std::uint8_t>() const;
};

template<typename T>
class PixelRgba
{
	static_assert(std::is_arithmetic_v<T>);

public:
	static inline unsigned int R = 0;
	static inline unsigned int G = 1;
	static inline unsigned int B = 2;
	static inline unsigned int A = 3;

public:
	std::array<T, 4> rgba; /* Using indices may allow for less code duplication. */

public:
	PixelRgba();
	PixelRgba(T r, T g, T b, T a);
};


/* 2D square grid with power-of-two side length, containing one CellType per cell.
 * The memory-mapping policy is given via a template parameter, allowing for testing
 * different approaches of cache-friendliness optimisations for example. */
template<typename CellType, typename Mapping = MappingRow>
class Grid
{
public:
	unsigned int side;
private:
	CellType* data;

public:
	Grid(unsigned int side);
	Grid(unsigned int side, std::function<CellType(GridCoords)> init_function);
	Grid(Grid const& grid);
	~Grid();
	
	CellType& access(GridCoords coords);
	CellType const& access(GridCoords coords) const;
	void const* raw_data() const;

	void output_as_bitmap(char const* output_file_path) const;
};

/* Returns a newly created grid that is a copy of the given grid_src but with evry pixel
 * mapped by the given convert_cell function. */
template<typename CellTypeSrc, typename MappingSrc, typename CellTypeDst, typename MappingDst>
Grid<CellTypeDst, MappingDst> convert_grid(
	Grid<CellTypeSrc, MappingSrc> const& grid_src,
	std::function<CellTypeDst(CellTypeSrc)> convert_cell);

template<typename CellType, typename MappingSrc, typename MappingDst>
Grid<CellType, MappingDst> convert_grid_mapping(Grid<CellType, MappingSrc> const& grid_src);

} /* StringArtRennes */

#endif /* HEADER_GRID_ */
