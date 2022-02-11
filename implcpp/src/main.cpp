
#include "grid.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>

using namespace sart;

/* The collection of all the inputs and parameters that are configurable by the user. */
class Config
{
public:
	std::string input_picture_name;

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

using LoadedPicture = Grid<std::uint8_t>;

/* Loads the picture of the given name in memory and returns it as a grid of pixels. */
LoadedPicture load_picture(std::string const& picture_name)
{
	using namespace std::literals::string_literals;

	std::cout << "Loading picture \"" << picture_name << "\"" << std::endl;

	int w, h;
	std::string filepath_dim = "../../impl/rawpics/"s + picture_name + ".dim"s;
	std::cout << "Reading picture dimensions from \"" << filepath_dim << "\"" << std::endl;
	std::ifstream file_dim(filepath_dim);
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
	std::ifstream file_raw(filepath_raw, std::ios::binary);
	if (not file_raw.is_open())
	{
		std::cerr << "Error: "
			<< "Could not open \"" << filepath_raw << "\"" << std::endl;
		std::exit(EXIT_FAILURE);
	}

	file_raw.seekg(0, std::ios::end);
	unsigned int raw_data_size = file_raw.tellg();
	file_raw.seekg(0, std::ios::beg);
	uint8_t* raw_data = new uint8_t[raw_data_size];
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

	LoadedPicture loaded_picture(side);
	for (unsigned int i = 0; i < raw_pixel_count; i++)
	{
		uint8_t raw_pixel_data = raw_data[i * raw_pixel_data_size];
		loaded_picture.access(row_mapping_inverse(i, side)) = raw_pixel_data;
	}
	std::cout << "Picture loaded" << std::endl;

	return loaded_picture;
}

int main(int argc, char** argv)
{
	Config config{argc, argv};

	LoadedPicture input_picture = load_picture(config.input_picture_name);

	return 0;
}
