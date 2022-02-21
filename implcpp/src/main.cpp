
#include "grid.hpp"
#include "bitmap.hpp"
#include "line.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <optional>

using namespace StringArtRennes;

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
	unsigned int const argc_unsigned = static_cast<unsigned int>(argc);
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
	std::string const filepath_dim = "../../impl/rawpics/"s + picture_name + ".dim"s;
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
	unsigned int const side = w;

	std::string const filepath_raw = "../../impl/rawpics/"s + picture_name + ".raw"s;
	std::cout << "Reading picture data from \"" << filepath_raw << "\"" << std::endl;
	std::ifstream file_raw{filepath_raw, std::ios::binary};
	if (not file_raw.is_open())
	{
		std::cerr << "Error: "
			<< "Could not open \"" << filepath_raw << "\"" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	file_raw.seekg(0, std::ios::end);
	unsigned int const raw_data_size = file_raw.tellg();
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
	unsigned int const raw_pixel_data_size = raw_data_size / (side * side);
	unsigned int const raw_pixel_count = raw_data_size / raw_pixel_data_size;
	if (raw_pixel_count != side * side)
	{
		std::cerr << "Error: "
			<< "Mismatch between the raw pixel count " << raw_pixel_count
			<< " expected to be " << side << " * " << side << " = " << side * side
			<< std::endl;
		std::exit(EXIT_FAILURE);
	}

	LoadedPictureGrid loaded_picture{side};
	for (unsigned int i = 0; i < raw_pixel_count; i++)
	{
		std::uint8_t const raw_pixel_data = raw_data[i * raw_pixel_data_size];
		loaded_picture.access(row_mapping_inverse(i, side)) = raw_pixel_data;
	}
	std::cout << "Picture loaded" << std::endl;

	return loaded_picture;
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
		bitmap_grid.access(GridCoords{x, y}) =
			PixelRgba<std::uint8_t>{gs, gs, gs, 255};
	}
	bitmap_grid.output_as_bitmap("test.bmp");

	BitmapComatibleGrid bitmap_grid_2{input_picture.side};
	for (unsigned int y = 0; y < bitmap_grid_2.side; y++)
	for (unsigned int x = 0; x < bitmap_grid_2.side; x++)
	{
		bitmap_grid_2.access(GridCoords{x, y}) =
			PixelRgba<std::uint8_t>{255, 255, 255, 255};
	}
	for (auto coords : RawLine{GridCoords{50, 50}, GridCoords{100, 200}})
	{
		bitmap_grid_2.access(coords) =
			PixelRgba<std::uint8_t>{0, 0, 0, 255};
	}
	bitmap_grid_2.output_as_bitmap("test_2.bmp");

	return 0;
}
