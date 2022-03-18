
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
#include <sstream>
#include <functional>
#include <random>

using namespace StringArtRennes;

/* The collection of all the inputs and parameters that are configurable by the user. */
class Config
{
public:
	bool test = false;
	std::optional<std::string> input_picture_name;
	std::optional<std::string> input_pinset_path;
	std::optional<unsigned int> circle_pins;
	std::optional<unsigned int> hd_sd_factor;

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
		if (strcmp(argv[i], "--test") == 0)
		{
			this->test = true;
			i += 1;
			continue;
		}
		else if (strcmp(argv[i], "-i") == 0)
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
		else if (strcmp(argv[i], "-p") == 0)
		{
			if (i + 1 < argc_unsigned)
			{
				this->input_pinset_path = std::string{argv[i + 1]};
			}
			else
			{
				std::cerr << "Error: "
					<< "Command line option \"" << argv[i] << "\" "
					<< "expects a file path to follow it as an argument" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			i += 2;
			continue;
		}
		else if (strcmp(argv[i], "-c") == 0)
		{
			if (i + 1 < argc_unsigned)
			{
				std::istringstream arg{argv[i + 1]};
				unsigned int arg_value;
				arg >> arg_value;
				this->circle_pins = arg_value;
			}
			else
			{
				std::cerr << "Error: "
					<< "Command line option \"" << argv[i] << "\" "
					<< "expects a number to follow it as an argument" << std::endl;
				std::exit(EXIT_FAILURE);
			}
			i += 2;
			continue;
		}
		else if (strcmp(argv[i], "-f") == 0)
		{
			if (i + 1 < argc_unsigned)
			{
				std::istringstream arg{argv[i + 1]};
				unsigned int arg_value;
				arg >> arg_value;
				this->hd_sd_factor = arg_value;
			}
			else
			{
				std::cerr << "Error: "
					<< "Command line option \"" << argv[i] << "\" "
					<< "expects a number to follow it as an argument" << std::endl;
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

using LoadedPictureGrid = Grid<PixelGs<std::uint8_t>, MappingRow>;

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
		loaded_picture.access(MappingRow::index_to_coords(i, side)) = raw_pixel_data;
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


using MappingMain = MappingZorder;
using TypeGsSd = std::uint8_t;
using GridGsSd = Grid<PixelGs<TypeGsSd>, MappingMain>;
using TypeCountHd = std::uint16_t;
using GridCountHd = Grid<PixelCount<TypeCountHd>, MappingMain>;

/* Returns the squared normalized distance between the given grayscale values.
 * The result is between 0.0f and 1.0f (included). */
float compute_pixel_sd_error_snd(
	PixelGs<TypeGsSd> target_pixel_sd,
	PixelGs<TypeGsSd> pixel_sd)
{
	constexpr float max_gs_sd = std::numeric_limits<TypeGsSd>::max();
	return square(
		static_cast<float>(target_pixel_sd.gs) / max_gs_sd -
		static_cast<float>(pixel_sd.gs) / max_gs_sd);
}

/* Retruns the delta (the variation) of the squared normalized distance between the
 * target garyscale value and the other one when the other one changes from old to new.
 * The result is between -1.0f and 1.0f (included). */
float compute_pixel_sd_error_snd_delta(
	PixelGs<TypeGsSd> target_pixel_sd,
	PixelGs<TypeGsSd> old_pixel_sd,
	PixelGs<TypeGsSd> new_pixel_sd)
{
	float const old_error = compute_pixel_sd_error_snd(target_pixel_sd, old_pixel_sd);
	float const new_error = compute_pixel_sd_error_snd(target_pixel_sd, new_pixel_sd);
	return new_error - old_error;
}

/* Returns the root-mean-square deviation between the given grids of pixel.
 * The result is between 0.0f and 1.0f (included). */
float compute_grid_rms(
	GridGsSd const& target_grid_sd,
	GridGsSd const& grid_sd)
{
	assert(target_grid_sd.side == grid_sd.side);
	unsigned int const side = target_grid_sd.side;

	float error_acc = 0.0f;
	for (GridCoords coords : RawRect{side})
	{
		error_acc += compute_pixel_sd_error_snd(
			target_grid_sd.access(coords),
			grid_sd.access(coords));
	}
	float const pixel_count = side * side;
	return std::sqrt(error_acc / pixel_count);
}

/* Returns a new grid every pixel of which represent the error
 * between the two analog pixels of the two given grids. */
GridGsSd compute_grid_error_as_grid(
	GridGsSd const& target_grid_sd,
	GridGsSd const& grid_sd)
{
	assert(target_grid_sd.side == grid_sd.side);
	unsigned int const side = target_grid_sd.side;

	constexpr float max_gs_sd = std::numeric_limits<TypeGsSd>::max();
	GridGsSd error_grid_sd{side, [&](GridCoords coords){
		float const error = compute_pixel_sd_error_snd(
			target_grid_sd.access(coords),
			grid_sd.access(coords));
		return static_cast<TypeGsSd>(error * max_gs_sd);
	}};
	return error_grid_sd;
}

/* Returns the given grayscale value but incremented (or decremented) to the nearest used value.
 * The set of used grayscale values depend on the hd_sd_factor.
 * If increment is false, then a decrementation is performed. */
TypeGsSd increment_or_decrement_gs_value(TypeGsSd gs, unsigned int hd_sd_factor, bool increment)
{
	constexpr float max_gs_sd = std::numeric_limits<TypeGsSd>::max();
	float const delta_sign = increment ? 1.0f : -1.0f;
	float const delta = max_gs_sd / static_cast<float>(square(hd_sd_factor)) * delta_sign;
	float const new_gs = static_cast<float>(gs) + delta;
	float const new_gs_rounded_clamped = std::max(0.0f, std::min(max_gs_sd, std::round(new_gs)));
	return static_cast<TypeGsSd>(new_gs_rounded_clamped);
}

inline TypeGsSd increment_gs_value(TypeGsSd gs, unsigned int hd_sd_factor)
{
	return increment_or_decrement_gs_value(gs, hd_sd_factor, true);
}

inline TypeGsSd decrement_gs_value(TypeGsSd gs, unsigned int hd_sd_factor)
{
	return increment_or_decrement_gs_value(gs, hd_sd_factor, false);
}


/* Returns a pair (error_delta, is_possible) about what would happen if we were to
 * draw (or erase if the erase parameter is true) the given line_hd on the given grid_hd
 * that would be kept in sync with the given grid_sd.
 * The error_delta result is how would change the error if the line was to be drawn/erased.
 * The is_possible result is false iff the line must not be drawn/erased (so that some
 * important invariant is not violated); it should not happen, but we never know.
 * In case is_possible is false, then error_delta is garbage (and would be irrelevant anyway). */
std::pair<float, bool> compute_un_error_delta_sd(
	GridGsSd const& target_sd, GridGsSd const& grid_sd, GridCountHd const& grid_hd,
	RawLine line_hd, bool erase)
{
	unsigned int const hd_sd_ratio = grid_hd.side / grid_sd.side;

	/* Lasy changes to apply to some SD pixel when it won't be accessed anymore.
	 * When these are applied (by update_un_error_delta_sd) it means that the modified SD pixel
	 * is the last one being accessed (while a new one is beginning to be accessed or when
	 * the computation is done). */
	GridCoords last_coords_sd = GridCoords{line_hd.a.x / hd_sd_ratio, line_hd.a.y / hd_sd_ratio};
	TypeGsSd last_new_gs_sd = grid_sd.access(last_coords_sd).gs;
	
	float un_error_delta_sd = 0;
	auto const update_un_error_delta_sd = [&]{
		float const local_un_error_delta_sd = compute_pixel_sd_error_snd_delta(
			target_sd.access(last_coords_sd),
			grid_sd.access(last_coords_sd),
			PixelGs<TypeGsSd>{last_new_gs_sd});
		un_error_delta_sd += local_un_error_delta_sd;
	};
	for (GridCoords coords : line_hd)
	{
		/* If we change of SD pixel,
		 * then apply the lazy modifications accumulated on the last SD pxiel. */
		GridCoords coords_sd = GridCoords{coords.x / hd_sd_ratio, coords.y / hd_sd_ratio};
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
			/* We want to avoid overflowing the overlap count on pixels.
			 * The current pixel cannot be modified in the indented way here and forcing it
			 * would lead us to lose the count on it, breaking an important invariant and
			 * possibly break the sync between grid_hd and grid_sd in the furure
			 * (which is to be avoided at all costs).
			 * We thus return a false second member to signify that this line
			 * is not to be drawn (or erased). */
			return std::make_pair(0, false);
		}
		else if ((not erase) && grid_hd.access(coords).count == 0)
		{
			/* One HD pixel would turn from white to black. */
			last_new_gs_sd = decrement_gs_value(last_new_gs_sd, hd_sd_ratio);
		}
		else if (erase && grid_hd.access(coords).count > 0)
		{
			/* One HD pixel would turn from black to white. */
			last_new_gs_sd = increment_gs_value(last_new_gs_sd, hd_sd_ratio);
		}
	}
	update_un_error_delta_sd();

	return std::make_pair(un_error_delta_sd, true);
}

/* Draw (or erase if the erase parameter is true) the given line_hd on the given grid_hd
 * that is kept in sync with the given grid_sd. */
void draw(
	GridGsSd& grid_sd, GridCountHd& grid_hd,
	RawLine line_hd, bool erase)
{
	unsigned int const hd_sd_ratio = grid_hd.side / grid_sd.side;
	for (GridCoords coords : line_hd)
	{
		GridCoords const coords_sd = GridCoords{coords.x / hd_sd_ratio, coords.y / hd_sd_ratio};
		constexpr auto max_count =
			std::numeric_limits<decltype(grid_hd.access(coords).count)>::max();
		if (((not erase) && grid_hd.access(coords).count == max_count) ||
			(erase && grid_hd.access(coords).count == 0))
		{
			/* This situaton should not happen if we do not draw/erase lines diagnosed
			 * as impossible by compute_un_error_delta_sd. See the code of this function
			 * for an explaination of why this must not be forced.
			 * Reaching this point should be considered as a bug. */
			assert(false);
		}
		else if (not erase)
		{
			if (grid_hd.access(coords).count++ == 0)
			{
				/* Keep in sync the grid_sd as one HD pixel turns from white to black. */
				grid_sd.access(coords_sd).gs = decrement_gs_value(
					grid_sd.access(coords_sd).gs, hd_sd_ratio);
			}
		}
		else if (erase)
		{
			if (--grid_hd.access(coords).count == 0)
			{
				/* Keep in sync the grid_sd as one HD pixel turns from black to white. */
				grid_sd.access(coords_sd).gs = increment_gs_value(
					grid_sd.access(coords_sd).gs, hd_sd_ratio);
			}
		}
	}
}

#if 0
GridCoords pin_coords_circle_pinset(unsigned int pin, unsigned int pin_count, unsigned int side)
{
	float const angle = tau * static_cast<float>(pin) / static_cast<float>(pin_count);
	return GridCoords{
		static_cast<unsigned int>(((std::cos(angle) + 1.0f) / 2.0f) * static_cast<float>(side - 1)),
		static_cast<unsigned int>(((std::sin(angle) + 1.0f) / 2.0f) * static_cast<float>(side - 1))};
}

using PinCoordsFunction = std::function<GridCoords(unsigned int, unsigned int, unsigned int)>;
#endif

int main(int argc, char** argv)
{
	Config config{argc, argv};

	LoadedPictureGrid const input_picture = load_picture(config.input_picture_name.value_or("popeye"));

	input_picture.output_as_bitmap("input.bmp");

	if (config.test)
	{
		std::cout << "test" << std::endl;
		return 0;
	}

	unsigned int const hd_sd_ratio = 8;
	unsigned int const side_sd = input_picture.side;
	unsigned int const side_hd = side_sd * hd_sd_ratio;
	GridGsSd const target_sd =
		convert_grid_mapping<PixelGs<TypeGsSd>, MappingRow, MappingMain>(input_picture);
	GridGsSd grid_sd{side_sd, [](GridCoords){return std::numeric_limits<TypeGsSd>::max();}};
	GridCountHd grid_hd{side_hd, [](GridCoords){return PixelCount<TypeCountHd>{0};}};

	
	/* Get the pin table. */
	std::vector<GridCoords> pins;
	if (config.input_pinset_path.has_value())
	{
		std::cout << "Getting pins from "
			<< "\"" << config.input_pinset_path.value() << "\"" << std::endl;
		std::fstream pinset_file{config.input_pinset_path.value()};
		while (pinset_file.good())
		{
			float x, y;
			char comma;
			pinset_file >> x >> comma >> y;
			pins.push_back(GridCoords{
				static_cast<unsigned int>(x * static_cast<float>(hd_sd_ratio)),
				static_cast<unsigned int>(y * static_cast<float>(hd_sd_ratio))});
		}
	}
	if (config.circle_pins.has_value() || (not config.input_pinset_path.has_value()))
	{
		unsigned int const circle_pin_count = config.circle_pins.value_or(512);
		std::cout << "Generating " << circle_pin_count << " pins in a circle shape" << std::endl;
		constexpr float tau = 6.28318530717f;
		for (unsigned int i = 0; i < circle_pin_count; i++)
		{
			float const angle = tau * static_cast<float>(i) / static_cast<float>(circle_pin_count);
			pins.push_back(GridCoords{
				static_cast<unsigned int>(((std::cos(angle) + 1.0f) / 2.0f)
					* static_cast<float>(side_hd - 1)),
				static_cast<unsigned int>(((std::sin(angle) + 1.0f) / 2.0f)
					* static_cast<float>(side_hd - 1))});
		}
	}
	unsigned int const pin_count = pins.size();
	std::cout << "Pin count: " << pin_count << std::endl;

	/* Get the line table. */
	std::vector<std::pair<unsigned int, RawLine>> lines;
	lines.reserve(pin_count * pin_count);
	for (unsigned int pin_index_a = 0; pin_index_a < pin_count; pin_index_a++)
	for (unsigned int pin_index_b = 0; pin_index_b < pin_count; pin_index_b++)
	{
		GridCoords const a = pins[pin_index_a];
		GridCoords const b = pins[pin_index_b];
		RawLine const line_hd{a, b};
		lines.push_back(std::make_pair(pin_index_a + pin_count * pin_index_b, line_hd));
	}
	std::cout << "Line count: " << lines.size() << std::endl;


	std::random_device random_device;
	std::mt19937 random_generator(random_device());

	float duration_total_seconds = 0.0f;
	unsigned int line_count = 0;
	std::vector<bool> strings_drawn;
	strings_drawn.resize(pin_count * pin_count);
	bool erase = false;
	std::cout << std::endl;
	while (true)
	{
		auto const time_iter_begin = std::chrono::system_clock::now();
		unsigned int const line_count_before = line_count;
		unsigned int impossible_count = 0;
		std::shuffle(lines.begin(), lines.end(), random_generator);
		for (auto const& [line_index, line_hd] : lines)
		{
			bool const is_drawn = strings_drawn[line_index];
			if (erase != is_drawn)
			{
				continue;
			}

			auto const [un_error_delta_sd, is_possible] =
				compute_un_error_delta_sd(target_sd, grid_sd, grid_hd, line_hd, erase);
			if (is_possible && (un_error_delta_sd < 0.0f))
			{
				draw(grid_sd, grid_hd, line_hd, erase);
				line_count += erase ? -1 : 1;
				strings_drawn[line_index] = not erase;
			}
			else if (not is_possible)
			{
				impossible_count++;
			}
		}
		auto const time_iter_end = std::chrono::system_clock::now();
		auto const duration_iter_milliseconds =
			std::chrono::duration_cast<std::chrono::milliseconds>(time_iter_end - time_iter_begin);
		float const duration_iter_seconds =
			static_cast<float>(duration_iter_milliseconds.count()) / 1000.0f;
		duration_total_seconds += duration_iter_seconds;

		int const line_count_diff =
			static_cast<int>(line_count) - static_cast<int>(line_count_before);
		std::cout << "Lines " << (erase ? "erased" : "drawn") << ": "
			<< std::abs(line_count_diff) << std::endl;
		std::cout << "Time taken: " << duration_iter_seconds << "s "
			<< "(total: " << duration_total_seconds << "s)" << std::endl;
		if (impossible_count > 0)
		{
			std::cout << "Warning: " << impossible_count << " lines were impossible to draw "
				<< "due to the too small max count per HD pixel" << std::endl;
		}
		grid_hd.output_as_bitmap("string_art_hd.bmp");
		grid_sd.output_as_bitmap("string_art_sd.bmp");
		compute_grid_error_as_grid(target_sd, grid_sd).output_as_bitmap("error.bmp");
		std::cout << "RMS: " << compute_grid_rms(target_sd, grid_sd) << std::endl;

		if (line_count_diff == 0)
		{
			std::cout << "No lines drawn or erased: halting execution" << std::endl;
			break;
		}

		erase = not erase;
	}

	return 0;
}
