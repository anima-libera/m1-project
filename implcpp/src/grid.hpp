
#ifndef HEADER_RANDOM_
#define HEADER_RANDOM_

#include <cstdint>

namespace sart {

class CoordsHD
{
public:
	std::uint16_t x;
	std::uint16_t y;

public:
	std::uint32_t to_index(std::size_t side);
};

/* Square grid with power-of-two side length, containing one T per cell.
 * The encapsulation allows for swapping the memory-mapping policy with new ones
 * easily for cache-friendliness optimisations. */
template<typename T>
class Grid
{
public:
	std::size_t side;
private:
	T* data;

public:
	Grid(std::size_t side);
	Grid(Grid const& grid);
	~Grid();
	T& access(CoordsHD coords);
};

} /* sart */

#endif /* HEADER_RANDOM_ */
