
#include "canvas.h"
#include "line.h"
#include "pinset.h"
#include "stringart.h"
#include "utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

/* Is using the given resolution safe from "gap lines" comming from floating point errors
 * that can ruin the rendering in some cases. */
int is_good_resolution(unsigned int resolution)
{
	for (unsigned int x = 0; x < resolution; x++)
	{
		const coords_grid_t original = {.x = x, .y = x};
		const coords_t tmp = coords_grid_to_coords(original, resolution);
		const coords_grid_t result = coords_to_coords_grid(tmp, resolution);
		if (original.x != result.x)
		{
			return 0;
		}
	}
	return 1;
}

/* Returns the smallest good resolution that is greater or equal to the given resolution.
 * This may be used with the _copy_upscale canvas functions to get a good resolution canvas. */
int make_resolution_good(unsigned int resolution)
{
	while (!is_good_resolution(resolution))
	{
		resolution++;
	}
	return resolution;
}

int main(int argc, const char** argv)
{
	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "--good-resolutions") == 0)
		{
			for (unsigned int resolution = 1; resolution < 4000; resolution++)
			{
				if (is_good_resolution(resolution))
				{
					printf("% 5d\t", resolution);
				}
			}
			printf("\n");
			return 0;
		}
	}

	struct rgba_t {unsigned char r, g, b, a;};
	typedef struct rgba_t rgba_t;

	const char* pic_name = NULL;
	if (argc >= 2)
	{
		pic_name = argv[1];
	}
	if (pic_name == NULL)
	{
		pic_name = "popeye";
	}

	unsigned int name_len = strlen(pic_name);
	char filepath_raw[11 + name_len + 4 + 1];
	char filepath_dim[11 + name_len + 4 + 1];
	sprintf(filepath_raw, "../rawpics/%s.raw", pic_name);
	sprintf(filepath_dim, "../rawpics/%s.dim", pic_name);

	printf("Reading dimensions from %s\n", filepath_dim);
	FILE* file_dim = fopen(filepath_dim, "r");
	unsigned int w, h;
	fscanf(file_dim, "%u %u", &w, &h);
	fclose(file_dim);

	printf("Reading raw pixel data from %s\n", filepath_raw);
	unsigned int buffer_size = w * h * sizeof(rgba_t);
	rgba_t* rgba_grid = malloc(buffer_size);
	FILE* file_raw = fopen(filepath_raw, "rb");
	fread(rgba_grid, buffer_size, 1, file_raw);
	fclose(file_raw);

	assert(w == h);
	canvas_float_t input_canvas = canvas_float_init_fill(w, 0.0f);
	for (unsigned int y = 0; y < input_canvas.resolution; y++)
	for (unsigned int x = 0; x < input_canvas.resolution; x++)
	{
		rgba_t rgba = rgba_grid[x + input_canvas.resolution * y];
		input_canvas.grid[x + input_canvas.resolution * y] =
			1.0f - (float)(rgba.r + rgba.g + rgba.b) / 3.0f / 255.0f;
	}

	const float current_canvas_background_gs = 0.0f;
	const unsigned int pre_resolution_factor = 4;

	canvas_float_t input_canvas_upscaled = canvas_float_copy_upscale(input_canvas,
		pre_resolution_factor);

	if (!is_good_resolution(input_canvas_upscaled.resolution))
	{
		const unsigned int bad_resolution = input_canvas_upscaled.resolution;
		const unsigned int good_resolution = make_resolution_good(bad_resolution);
		canvas_float_t good_canvas = canvas_float_copy_expand(input_canvas_upscaled,
			good_resolution, current_canvas_background_gs);
		canvas_float_cleanup(input_canvas_upscaled);
		input_canvas_upscaled = good_canvas;

		printf("Bad resolution %d detected, upscaled to resolution %d.\n",
			bad_resolution, good_resolution);
		printf("Note: "
			"The --good-resolutions command line option asks for a list of good resolutions.\n");
	}

#if 0
	canvas_float_t importance_canvas = canvas_float_init(input_canvas_upscaled.resolution);
	for (unsigned int y = 0; y < importance_canvas.resolution; y++)
	for (unsigned int x = 0; x < importance_canvas.resolution; x++)
	{
		const coords_grid_t coords_grid = {.x = x, .y = y};
		const coords_t coords = coords_grid_to_coords(coords_grid, importance_canvas.resolution);
		const float importance = dist(coords.x, coords.y, 0.62f, 0.57f) < 0.15f ? 1.0f : 0.01f;
		importance_canvas.grid[x + importance_canvas.resolution * y] = importance;
	}
#elif 0
	canvas_float_t importance_canvas = canvas_float_init(input_canvas_upscaled.resolution);
	for (unsigned int y = 0; y < importance_canvas.resolution; y++)
	for (unsigned int x = 0; x < importance_canvas.resolution; x++)
	{
		const coords_grid_t coords_grid = {.x = x, .y = y};
		const coords_t coords = coords_grid_to_coords(coords_grid, importance_canvas.resolution);
		const float importance = dist(coords.x, coords.y, 0.657f, 0.5f) < 0.11f ? 1.0f : 0.01f;
		importance_canvas.grid[x + importance_canvas.resolution * y] = importance;
	}
#else
	canvas_float_t importance_canvas = canvas_float_init_fill(input_canvas_upscaled.resolution,
		1.0f);
#endif

	perform_string_art((string_art_input_t){
		.input_canvas = input_canvas_upscaled,
		.importance_canvas = importance_canvas,
		.current_canvas_background_gs = current_canvas_background_gs,
		.line_color = (gs_op_t){.gs = 1.0f, .op = 1.0f},
		.error_formula = ERROR_FORMULA_DIFF,
		.resolution_factor = 1,
		.pinset = pinset_generate_circle_center(256),
		.line_pool_length = 300,
		.iteration_max_number = 60000,
	});

	return 0;
}

/* Old test code that might be useful. */
#if 0
canvas_gs_op_t canvas_sd;
canvas_sd.resolution = 128;
canvas_sd.grid = malloc(canvas_sd.resolution * canvas_sd.resolution * sizeof(gs_op_t));

canvas_gs_op_t canvas_hd;
canvas_hd.resolution = 128 * 4;
canvas_hd.grid = malloc(canvas_hd.resolution * canvas_hd.resolution * sizeof(gs_op_t));

pinset_t pinset = pinset_generate_square(30);

for (unsigned int i = 0; i < pinset.pin_count; i++)
for (unsigned int j = i + 1; j < pinset.pin_count; j++)
{
	line_pixels_t line_pixels_hd = line_coords_to_line_pixels(
		(line_coords_t){
			.coords_a = pinset.pin_array[i],
			.coords_b = pinset.pin_array[j],
			.color = {.gs = 1.0f, .op = 1.0f}},
		canvas_hd.resolution,
		line_plot_pixels_mid_point);

	line_pixels_t line_pixels_sd = line_pixels_downscale_hd_to_sd(line_pixels_hd,
		canvas_sd.resolution);

	canvas_gs_op_draw_line_pixels(canvas_hd, line_pixels_hd);
	canvas_gs_op_draw_line_pixels(canvas_sd, line_pixels_sd);

	line_pixels_cleanup(line_pixels_hd);
	line_pixels_cleanup(line_pixels_sd);
}

canvas_gs_op_output_bmp(canvas_hd, 0.0f, "test_hd.bmp");
canvas_gs_op_output_bmp(canvas_sd, 0.0f, "test_sd.bmp");
#endif
