
#ifndef HEADER_BITMAP_
#define HEADER_BITMAP_

#include "grid.hpp"
#include <cstdint>

namespace StringArtRennes
{

/* Grid type having a memory representation that can be
 * directly emitted as a bitmap data section. */
using BitmapComatibleGrid = Grid<PixelRgba<std::uint8_t>, MappingRow>;

/* Performs the emission of a grid as a bitmap file, at the given path. */
void output_bitmap(BitmapComatibleGrid const& grid, char const* output_file_path);

} /* StringArtRennes */

#endif /* HEADER_BITMAP_ */
