
#include "grid.hpp"
#include "bitmap.hpp"
#include "line.hpp"
#include <iostream>
#include <string>
#include <fstream>
#include <cstring>
#include <optional>
#include <limits>
#include <cassert>
#include <cmath>
#include <vector>
#include <algorithm>
#include <chrono>

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
	delete[] raw_data;
	std::cout << "Picture loaded" << std::endl;

	return loaded_picture;
}


template<typename T>
static inline T square(T x)
{
	return x * x;
}

using TypeGsSd = std::uint8_t;
using TypeCountHd = std::uint16_t;

float compute_pixel_sd_error_squared(
	PixelGs<TypeGsSd> target_pixel_sd,
	PixelGs<TypeGsSd> pixel_sd)
{
	constexpr float max_gs_sd = std::numeric_limits<TypeGsSd>::max();
	return square(
		static_cast<float>(target_pixel_sd.gs) / max_gs_sd -
		static_cast<float>(pixel_sd.gs) / max_gs_sd);
}

float compute_pixel_sd_error_delta(
	PixelGs<TypeGsSd> target_pixel_sd,
	PixelGs<TypeGsSd> old_pixel_sd,
	PixelGs<TypeGsSd> new_pixel_sd)
{
	float const old_error = compute_pixel_sd_error_squared(target_pixel_sd, old_pixel_sd);
	float const new_error = compute_pixel_sd_error_squared(target_pixel_sd, new_pixel_sd);
	return new_error - old_error;
}

float compute_rms(
	Grid<PixelGs<TypeGsSd>, row_mapping> const& target_grid_sd,
	Grid<PixelGs<TypeGsSd>, row_mapping> const& grid_sd)
{
	float error_acc = 0.0f;
	for (unsigned int y = 0; y < target_grid_sd.side; y++)
	for (unsigned int x = 0; x < target_grid_sd.side; x++)
	{
		error_acc += compute_pixel_sd_error_squared(
			target_grid_sd.access(GridCoords{x, y}),
			grid_sd.access(GridCoords{x, y}));
	}
	float const pixel_count = target_grid_sd.side * target_grid_sd.side;
	return std::sqrt(error_acc / pixel_count);
}

TypeGsSd change_gs(TypeGsSd gs, unsigned int f, bool increment)
{
	constexpr float max_gs_sd = std::numeric_limits<TypeGsSd>::max();
	float const delta_sign = increment ? 1.0f : -1.0f;
	float const delta = max_gs_sd / static_cast<float>(f * f) * delta_sign;
	float const new_gs = static_cast<float>(gs) + delta;
	float const new_gs_rounded_clamped = std::max(0.0f, std::min(max_gs_sd, std::round(new_gs)));
	return static_cast<TypeGsSd>(new_gs_rounded_clamped);
}

std::pair<float, bool> compute_un_error_delta_sd(
	Grid<PixelGs<TypeGsSd>, row_mapping> const& input_sd,
	Grid<PixelGs<TypeGsSd>, row_mapping> const& grid_sd,
	Grid<PixelCount<TypeCountHd>, row_mapping> const& grid_hd,
	RawLine line_hd,
	bool erase)
{
	unsigned int const f = grid_hd.side / grid_sd.side;
	GridCoords last_coords_sd = GridCoords{line_hd.a.x / f, line_hd.a.y / f};
	TypeGsSd last_new_gs_sd = grid_sd.access(last_coords_sd).gs;
	float un_error_delta_sd = 0;
	constexpr float max_gs_sd =
		std::numeric_limits<decltype(grid_sd.access(last_coords_sd).gs)>::max();
	auto update_un_error_delta_sd = [&]{
		float const local_un_error_delta_sd = compute_pixel_sd_error_delta(
			input_sd.access(last_coords_sd),
			grid_sd.access(last_coords_sd),
			PixelGs<TypeGsSd>{last_new_gs_sd});
		un_error_delta_sd += local_un_error_delta_sd;
	};
	for (GridCoords coords : line_hd)
	{
		GridCoords coords_sd = GridCoords{coords.x / f, coords.y / f};
		if (coords_sd != last_coords_sd)
		{
			update_un_error_delta_sd();
			
			last_new_gs_sd = grid_sd.access(coords_sd).gs;
			last_coords_sd = coords_sd;
		}
		constexpr auto max_count =
			std::numeric_limits<decltype(grid_hd.access(coords).count)>::max();
		if (((not erase) && grid_hd.access(coords).count == max_count) ||
			(erase && grid_hd.access(coords).count == 0))
		{
			/* We want to avoid overflowing the overlap count on pixels. */
			return std::make_pair(0, false);
		}
		else if ((not erase) && grid_hd.access(coords).count == 0)
		{
			last_new_gs_sd = change_gs(last_new_gs_sd, f, false);
		}
		else if (erase && grid_hd.access(coords).count > 0)
		{
			last_new_gs_sd = change_gs(last_new_gs_sd, f, true);
		}
	}
	update_un_error_delta_sd();

	return std::make_pair(un_error_delta_sd, true);
}

void draw(
	Grid<PixelGs<TypeGsSd>, row_mapping>& input_sd,
	Grid<PixelGs<TypeGsSd>, row_mapping>& grid_sd,
	Grid<PixelCount<TypeCountHd>, row_mapping>& grid_hd,
	RawLine line_hd,
	bool erase)
{
	unsigned int const f = grid_hd.side / grid_sd.side;
	for (GridCoords coords : line_hd)
	{
		GridCoords coords_sd = GridCoords{coords.x / f, coords.y / f};
		constexpr auto max_gs_sd =
			std::numeric_limits<decltype(grid_sd.access(coords_sd).gs)>::max();
		constexpr auto max_count =
			std::numeric_limits<decltype(grid_hd.access(coords).count)>::max();
		if (((not erase) && grid_hd.access(coords).count == max_count) ||
			(erase && grid_hd.access(coords).count == 0))
		{
			assert(false);
		}
		else if (not erase)
		{
			if (grid_hd.access(coords).count++ == 0)
			{
				grid_sd.access(coords_sd).gs = change_gs(grid_sd.access(coords_sd).gs, f, false);
			}
		}
		else if (erase)
		{
			if (--grid_hd.access(coords).count == 0)
			{
				grid_sd.access(coords_sd).gs = change_gs(grid_sd.access(coords_sd).gs, f, true);
			}
		}
	}
}

constexpr float tau = 6.28318530717f;

GridCoords pin_coords(unsigned int pin, unsigned int pin_count, unsigned int side)
{
	float const angle = tau * static_cast<float>(pin) / static_cast<float>(pin_count);
	return GridCoords{
		((std::cos(angle) + 1.0f) / 2.0f) * static_cast<float>(side - 1),
		((std::sin(angle) + 1.0f) / 2.0f) * static_cast<float>(side - 1)};
}


int main(int argc, char** argv)
{
	Config config{argc, argv};

	LoadedPictureGrid input_picture = load_picture(config.input_picture_name.value_or("popeye"));

	BitmapComatibleGrid bitmap_grid{input_picture.side};
	for (unsigned int y = 0; y < bitmap_grid.side; y++)
	for (unsigned int x = 0; x < bitmap_grid.side; x++)
	{
		bitmap_grid.access(GridCoords{x, y}) =
			static_cast<PixelRgba<std::uint8_t>>(input_picture.access(GridCoords{x, y}));
	}
	bitmap_grid.output_as_bitmap("input.bmp");

	unsigned int const f = 8;
	unsigned int const side_sd = input_picture.side;
	unsigned int const side_hd = side_sd * f;
	Grid<PixelGs<TypeGsSd>, row_mapping> input_sd{side_sd};
	for (unsigned int y = 0; y < input_sd.side; y++)
	for (unsigned int x = 0; x < input_sd.side; x++)
	{
		input_sd.access(GridCoords{x, y}) = input_picture.access(GridCoords{x, y});
	}
	Grid<PixelGs<TypeGsSd>, row_mapping> grid_sd{side_sd};
	for (unsigned int y = 0; y < grid_sd.side; y++)
	for (unsigned int x = 0; x < grid_sd.side; x++)
	{
		grid_sd.access(GridCoords{x, y}) = PixelGs<TypeGsSd>{std::numeric_limits<TypeGsSd>::max()};
	}
	Grid<PixelCount<TypeCountHd>, row_mapping> grid_hd{side_hd};
	for (unsigned int y = 0; y < grid_hd.side; y++)
	for (unsigned int x = 0; x < grid_hd.side; x++)
	{
		grid_hd.access(GridCoords{x, y}) = PixelCount<TypeCountHd>{0};
	}

	unsigned int const pin_count = 512;
	std::vector<std::pair<std::uint16_t, std::uint16_t>> pin_pairs;
	pin_pairs.reserve(pin_count * pin_count);
	for (unsigned int pin_a = 0; pin_a < pin_count; pin_a++)
	for (unsigned int pin_b = 0; pin_b < pin_count; pin_b++)
	{
		pin_pairs.push_back(std::make_pair(pin_a, pin_b));
	}

	unsigned int line_count = 0;
	std::vector<bool> strings_drawn;
	strings_drawn.resize(pin_count * pin_count);
	bool erase = false;
	while (true)
	{
		auto const time_iter_begin = std::chrono::system_clock::now();
		unsigned int const line_count_before = line_count;
		unsigned int impossible_count = 0;
		std::random_shuffle(pin_pairs.begin(), pin_pairs.end());
		for (auto [pin_a, pin_b] : pin_pairs)
		{
			bool const is_drawn = strings_drawn[pin_a + pin_count * pin_b];
			if (erase != is_drawn)
			{
				continue;
			}
			GridCoords const a = pin_coords(pin_a, pin_count, side_hd);
			GridCoords const b = pin_coords(pin_b, pin_count, side_hd);
			//std::cout << a.x << ", " << a.y << " -- " << b.x << ", " << b.y << ": " << std::flush;
			RawLine const line_hd{a, b};
			auto const [un_error_delta_sd, is_possible] =
				compute_un_error_delta_sd(input_sd, grid_sd, grid_hd, line_hd, erase);
			#if 0
			if (not is_possible)
			{
				std::cout << pin_a << " -- " << pin_b << ": " << "impossible" << std::endl;
			}
			else
			{
				std::cout << pin_a << " -- " << pin_b << ": " << un_error_delta_sd << std::endl;
			}
			#endif
			if (is_possible && (un_error_delta_sd < 0))
			{
				draw(input_sd, grid_sd, grid_hd, line_hd, erase);
				line_count += erase ? -1 : 1;
				strings_drawn[pin_a + pin_count * pin_b] = not erase;
			}
			else if (not is_possible)
			{
				impossible_count++;
			}
		}
		auto const time_iter_end = std::chrono::system_clock::now();
		auto const duration_iter =
			std::chrono::duration_cast<std::chrono::milliseconds>(time_iter_end - time_iter_begin);

		int line_count_diff = static_cast<int>(line_count) - static_cast<int>(line_count_before);
		std::cout << "Lines " << (erase ? "erased" : "drawn") << ": "
			<< std::abs(line_count_diff) << std::endl;
		std::cout << "Time taken: " << duration_iter.count() << "ms" << std::endl;
		if (impossible_count > 0)
		{
			std::cout << "Warning: " << impossible_count << " lines were impossible to draw "
				<< "due to the too small max count per HD pixel" << std::endl;
		}
		grid_hd.output_as_bitmap("string_art_hd.bmp");
		grid_sd.output_as_bitmap("string_art_sd.bmp");
		std::cout << "RMS: " << compute_rms(input_sd, grid_sd) << std::endl;

		if (line_count_diff == 0)
		{
			std::cout << "No lines drawn or erased: halting execution" << std::endl;
			break;
		}

		erase = not erase;
	}

	return 0;
}
