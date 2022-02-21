
#include "bitmap.hpp"
#include <string>
#include <iostream>
#include <fstream>
#include <climits>

namespace sart
{

static_assert(sizeof(PixelRgba<std::uint8_t>) == 4, "Bug");
static_assert(CHAR_BIT == 8, "Unsupported target plateform");

/* Bitmap file format header (without the magic number), see
 * https://en.wikipedia.org/wiki/BMP_file_format and https://www.digicamsoft.com/bmp/bmp.html
 * for more information. */
struct BitmapHeader
{
public:
	/* BITMAPFILEHEADER members (without the magic number). */
	/* The magic number 'BM' should be written just before this header,
	 * without padding (that is why it is not a member of this struct declaration). */
	uint32_t file_size;
	uint16_t reserved_1, reserved_2; /* Can both be 0. */
	uint32_t file_offset_to_pixel_array;

	/* BITMAPINFOHEADER members. */
	uint32_t dbi_header_size; /* Size of the BITMAPINFOHEADER header only. */
	int32_t image_width;
	int32_t image_height;
	uint16_t planes; /* Must be 1. */
	uint16_t bits_per_pixel;
	uint32_t compression; /* 0 means no compression, 3 means BI_BITFIELDS. */
	uint32_t image_size;
	int32_t x_pixel_per_meter;
	int32_t y_pixel_per_meter;
	uint32_t colors_in_color_table; /* Can be 0. */
	uint32_t important_color_count; /* Can be 0. */
};

void output_bitmap(BitmapComatibleGrid& grid, std::string const& output_file_path)
{
	std::cout << "Outputing picture as bitmap \"" << output_file_path << "\"" << std::endl;

	std::ofstream file{output_file_path, std::ios::binary};
	if (not file.is_open())
	{
		std::cerr << "Error: "
			<< "Could not open \"" << output_file_path << "\"" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	const unsigned int magic_number_size = 2;
	const unsigned int channel_mask_size = 3 * sizeof(PixelRgba<std::uint8_t>);
	const unsigned int image_size = grid.side * grid.side * sizeof(PixelRgba<std::uint8_t>);

	/* Magic number. */
	file.write("BM", magic_number_size);

	/* Header (using the BITMAPINFOHEADER header version). */
	BitmapHeader header;
	header.file_size =
		magic_number_size + sizeof(BitmapHeader) + channel_mask_size + image_size;
	header.file_offset_to_pixel_array =
		magic_number_size + sizeof(BitmapHeader) + channel_mask_size;
	header.dbi_header_size =
		sizeof(BitmapHeader) - offsetof(BitmapHeader, dbi_header_size);
	header.image_width = grid.side;
	header.image_height = grid.side;
	header.planes = 1;
	header.bits_per_pixel = sizeof(PixelRgba<std::uint8_t>) * 8;
	header.compression = 3; /* BI_BITFIELDS */
		/* Note:
		* BI_BITFIELDS is used here instead of BI_ALPHABITFIELDS due ot a lack
		* of support for the latter option. */
	header.image_size = image_size;
	header.x_pixel_per_meter = 5000; /* Is it good ? It seems to be ok... */
	header.y_pixel_per_meter = 5000; /* Is it good ? It seems to be ok... */
	header.colors_in_color_table = 0; /* For some reason it must be 0, even though it is 3. */
	header.important_color_count = 0;
	file.write(reinterpret_cast<char const*>(&header), sizeof header);

	/* Channel masks. */
	const PixelRgba<std::uint8_t> mask_r{0xff, 0x00, 0x00, 0x00};
	const PixelRgba<std::uint8_t> mask_g{0x00, 0xff, 0x00, 0x00};
	const PixelRgba<std::uint8_t> mask_b{0x00, 0x00, 0xff, 0x00};
	file.write(reinterpret_cast<char const*>(&mask_r), sizeof(PixelRgba<std::uint8_t>));
	file.write(reinterpret_cast<char const*>(&mask_g), sizeof(PixelRgba<std::uint8_t>));
	file.write(reinterpret_cast<char const*>(&mask_b), sizeof(PixelRgba<std::uint8_t>));
	
	/* Pixel array. As the size of a pixel is 32 bits, the padding between rows
	 * of the bitmap pixel array to alling on 32 bits is of size 0
	 * and can be ignored. */
	file.write(reinterpret_cast<char const*>(grid.raw_data()),
		grid.side * grid.side * sizeof(PixelRgba<std::uint8_t>));
}

} /* sart */
