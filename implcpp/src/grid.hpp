
#ifndef HEADER_RANDOM_
#define HEADER_RANDOM_

#include <cstdint>

namespace sart {

class GridCoords
{
public:
	std::uint16_t x;
	std::uint16_t y;

public:
	GridCoords(std::uint16_t x, std::uint16_t y);
};

using Mapping = std::uint32_t (*)(GridCoords coords, unsigned int side);

std::uint32_t row_mapping(GridCoords coords, unsigned int side);
GridCoords row_mapping_inverse(std::uint32_t index, unsigned int side);

std::uint32_t zorder_mapping(GridCoords coords, unsigned int side);

/* Square grid with power-of-two side length, containing one T per cell.
 * The encapsulation allows for swapping the memory-mapping policy with new ones
 * easily for cache-friendliness optimisations for example. */
template<typename CellType, Mapping mapping_parameter = row_mapping>
class Grid
{
public:
	static inline Mapping mapping = mapping_parameter;

public:
	unsigned int side;
private:
	CellType* data;

public:
	Grid(unsigned int side);
	Grid(Grid const& grid);
	~Grid();
	CellType& access(GridCoords coords);
};

} /* sart */

#endif /* HEADER_RANDOM_ */
