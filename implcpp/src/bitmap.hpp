
#ifndef HEADER_BITMAP_
#define HEADER_BITMAP_

#include "grid.hpp"
#include <cstdint>
#include <string>

namespace sart
{

using BitmapComatibleGrid = Grid<PixelRgba<std::uint8_t>, row_mapping>;

void output_bitmap(BitmapComatibleGrid& grid, std::string const& output_file_path);

} /* sart */

#endif /* HEADER_BITMAP_ */
