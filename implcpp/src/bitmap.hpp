
#ifndef HEADER_BITMAP_
#define HEADER_BITMAP_

#include "grid.hpp"
#include <cstdint>

namespace StringArtRennes
{

using BitmapComatibleGrid = Grid<PixelRgba<std::uint8_t>, row_mapping>;

void output_bitmap(BitmapComatibleGrid const& grid, char const* output_file_path);

} /* StringArtRennes */

#endif /* HEADER_BITMAP_ */
