
#include "canvas.h"
#include "line.h"
#include "pinset.h"
#include "stringart.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>

int main(void)
{
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

	#if 0
	canvas_float_t target_canvas = canvas_float_init_fill(128, 0.0f);
	for (unsigned int y = 0; y < target_canvas.resolution; y++)
	for (unsigned int x = 0; x < target_canvas.resolution; x++)
	{
		target_canvas.grid[x + target_canvas.resolution * y] =
			(float)x / (float)target_canvas.resolution;
	}
	for (unsigned int y = 40; y < 57; y++)
	for (unsigned int x = 30; x < 45; x++)
	{
		target_canvas.grid[x + target_canvas.resolution * y] = 1.0f;
	}
	#endif

	struct rgba_t {unsigned char r, g, b, a;};
	typedef struct rgba_t rgba_t;

	const char* pic_name = "popeye";

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
	canvas_float_t target_canvas = canvas_float_init_fill(w, 0.0f);
	for (unsigned int y = 0; y < target_canvas.resolution; y++)
	for (unsigned int x = 0; x < target_canvas.resolution; x++)
	{
		rgba_t rgba = rgba_grid[x + target_canvas.resolution * y];
		target_canvas.grid[x + target_canvas.resolution * y] =
			1.0f - (float)(rgba.r + rgba.g + rgba.b) / 3.0f / 255.0f;
	}

	perform_string_art((string_art_input_t){
		.target_canvas = target_canvas,
		.current_canvas_background_gs = 0.0f,
		.line_color = (gs_op_t){.gs = 1.0f, 0.5f},
		.error_formula = ERROR_FORMULA_DIFF,
		.resolution_factor = 4,
		.pinset = pinset_generate_circle(256),
		.line_pool_length = 500,
		.iteration_max_number = 60000,
	});

	return 0;
}
