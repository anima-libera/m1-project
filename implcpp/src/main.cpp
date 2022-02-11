
#include "grid.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <climits>
#include <optional>

using namespace sart;

/* The collection of all the inputs and parameters that are configurable by the user. */
class Config
{
public:
	std::optional<std::string> input_picture_name;

public:
	/* Constructor taking the raw arguments of main. */
	Config(int argc, char** argv);
};

Config::Config(int argc, char** argv)
{
	const unsigned int argc_unsigned = static_cast<unsigned int>(argc);
	unsigned int i = 1;
	while (i < argc_unsigned)
	{
		if (strcmp(argv[i], "-i") == 0)
		{
			if (i + 1 < argc_unsigned)
			{
				this->input_picture_name = std::string{argv[i + 1]};
			}
			else
			{
				std::cerr << "Error: "
					<< "Command line option \"" << argv[i] << "\" "
					<< "expects a picture name to follow it as an argument" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			i += 2;
			continue;
		}
		else
		{
			std::cerr << "Error: "
				<< "Unknown command line option \"" << argv[i] << "\"" << std::endl;
			std::exit(EXIT_FAILURE);
			i++;
		}
	}
}

using LoadedPictureGrid = Grid<PixelGs<std::uint8_t>, row_mapping>;

/* Loads the picture of the given name in memory and returns it as a grid of pixels. */
LoadedPictureGrid load_picture(std::string const& picture_name)
{
	using namespace std::literals::string_literals;

	std::cout << "Loading picture \"" << picture_name << "\"" << std::endl;

	int w, h;
	std::string filepath_dim = "../../impl/rawpics/"s + picture_name + ".dim"s;
	std::cout << "Reading picture dimensions from \"" << filepath_dim << "\"" << std::endl;
	std::ifstream file_dim{filepath_dim};
	if (not file_dim.is_open())
	{
		std::cerr << "Error: "
			<< "Could not open \"" << filepath_dim << "\"" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	file_dim >> w >> h;
	file_dim.close();
	std::cout << "Picture dimensions: " << w << "x" << h << std::endl;

	if (w != h)
	{
		std::cerr << "Error: "
			<< "Picture dimensions do not describe a square" << std::endl;
		std::exit(EXIT_FAILURE);
	}
	unsigned int side = w;

	std::string filepath_raw = "../../impl/rawpics/"s + picture_name + ".raw"s;
	std::cout << "Reading picture data from \"" << filepath_raw << "\"" << std::endl;
	std::ifstream file_raw{filepath_raw, std::ios::binary};
	if (not file_raw.is_open())
	{
		std::cerr << "Error: "
			<< "Could not open \"" << filepath_raw << "\"" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	file_raw.seekg(0, std::ios::end);
	unsigned int raw_data_size = file_raw.tellg();
	file_raw.seekg(0, std::ios::beg);
	std::uint8_t* raw_data = new std::uint8_t[raw_data_size];
	file_raw.read(reinterpret_cast<char*>(raw_data), raw_data_size);
	file_raw.close();

	if (raw_data_size % (side * side) != 0)
	{
		std::cerr << "Error: "
			<< "Mismatch between the raw data size " << raw_data_size
			<< " expected to be a multiple of the number of pixels " << (side * side)
			<< std::endl;
		std::exit(EXIT_FAILURE);
	}
	unsigned int raw_pixel_data_size = raw_data_size / (side * side);
	unsigned int raw_pixel_count = raw_data_size / raw_pixel_data_size;
	if (raw_pixel_count != side * side)
	{
		std::cerr << "Error: "
			<< "Mismatch between the raw pixel count " << raw_pixel_count
			<< " expected to be " << side << " * " << side << " = " << side * side
			<< std::endl;
		std::exit(EXIT_FAILURE);
	}

	LoadedPictureGrid loaded_picture(side);
	for (unsigned int i = 0; i < raw_pixel_count; i++)
	{
		std::uint8_t raw_pixel_data = raw_data[i * raw_pixel_data_size];
		loaded_picture.access(row_mapping_inverse(i, side)) = raw_pixel_data;
	}
	std::cout << "Picture loaded" << std::endl;

	return loaded_picture;
}

using BitmapComatibleGrid = Grid<PixelRgba<std::uint8_t>, row_mapping>;
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

int main(int argc, char** argv)
{
	Config config{argc, argv};

	LoadedPictureGrid input_picture = load_picture(config.input_picture_name.value_or("popeye"));

	BitmapComatibleGrid bitmap_grid{input_picture.side};
	for (unsigned int y = 0; y < bitmap_grid.side; y++)
	for (unsigned int x = 0; x < bitmap_grid.side; x++)
	{
		std::uint8_t gs = input_picture.access(
			GridCoords{
				static_cast<std::uint16_t>(x % input_picture.side),
				static_cast<std::uint16_t>(y % input_picture.side)}
			).gs;
		bitmap_grid.access(GridCoords(x, y)) =
			PixelRgba<std::uint8_t>{gs, gs, gs, 255};
	}
	output_bitmap(bitmap_grid, "test.bmp");

	return 0;
}
