
#ifndef HEADER_GRID_
#define HEADER_GRID_

#include <cstdint>
#include <array>
#include <type_traits>

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
	GridCoords(std::uint16_t x, std::uint16_t y);
	bool operator==(GridCoords const& right) const;
	bool operator!=(GridCoords const& right) const;
};


/* Mapping of the cells of a 2D square grid in 1D memory representation. */
using Mapping = std::uint32_t (*)(GridCoords coords, unsigned int side);

/* Row mapping, the classic. */
std::uint32_t row_mapping(GridCoords coords, unsigned int side);
GridCoords row_mapping_inverse(std::uint32_t index, unsigned int side);

/* Z-Order mapping, see implementation for details. */
std::uint32_t zorder_mapping(GridCoords coords, unsigned int side);


template<typename T>
class PixelRgba;

template<typename T>
class PixelGs
{
	static_assert(std::is_arithmetic_v<T>);

public:
	T gs; /* Grayscale value. */

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
	T count; /* Some count. */

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
template<typename CellType, Mapping mapping_parameter = row_mapping>
class Grid
{
public:
	static inline constexpr Mapping mapping = mapping_parameter;

public:
	unsigned int side;
private:
	CellType* data;

public:
	Grid(unsigned int side);
	Grid(Grid const& grid);
	~Grid();
	
	CellType& access(GridCoords coords);
	CellType const& access(GridCoords coords) const;
	void const* raw_data() const;

	void output_as_bitmap(char const* output_file_path) const;
};

} /* StringArtRennes */

#endif /* HEADER_GRID_ */
