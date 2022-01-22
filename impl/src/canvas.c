
#include "canvas.h"
#include "basics.h"
#include "bmp.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#define INT_COORDS_IS_IN_BOUNDS(coords_gird_, resolution_) \
	(0 <= (coords_gird_).x && (coords_gird_).x < (int)(resolution_) && \
		0 <= (coords_gird_).y && (coords_gird_).y < (int)(resolution_))

static gs_op_t gs_op_t_add(gs_op_t a, gs_op_t b)
{
	return (gs_op_t){.gs = a.gs + b.gs, .op = a.op + b.op};
}
static gs_op_t gs_op_t_div(gs_op_t x, float d)
{
	return (gs_op_t){.gs = x.gs / d, .op = x.op / d};
}
static float float_add(float a, float b)
{
	return a + b;
}
static float float_div(float x, float d)
{
	return x / d;
}

#define DEFINE_CANVAS(canvas_type_, function_prefix_, element_type_) \
	\
	canvas_type_ function_prefix_##_init(unsigned int resolution) \
	{ \
		canvas_type_ canvas; \
		canvas.resolution = resolution; \
		canvas.grid = malloc(resolution * resolution * sizeof(element_type_)); \
		return canvas; \
	} \
	\
	void function_prefix_##_cleanup(canvas_type_ canvas) \
	{ \
		free(canvas.grid); \
	} \
	\
	canvas_type_ function_prefix_##_init_fill(unsigned int resolution, \
		element_type_ filling_element) \
	{ \
		canvas_type_ canvas = function_prefix_##_init(resolution); \
		for (unsigned int i = 0; i < resolution * resolution; i++) \
		{ \
			canvas.grid[i] = filling_element; \
		} \
		return canvas; \
	} \
	\
	canvas_type_ function_prefix_##_copy(canvas_type_ canvas) \
	{ \
		canvas_type_ copy; \
		copy.resolution = canvas.resolution; \
		const unsigned int grid_linear_length = copy.resolution * copy.resolution; \
		copy.grid = malloc(grid_linear_length * sizeof(element_type_)); \
		memcpy(copy.grid, canvas.grid, grid_linear_length * sizeof(element_type_)); \
		return copy; \
	} \
	\
	canvas_type_ function_prefix_##_copy_upscale(canvas_type_ canvas_sd, \
		unsigned int resolution_factor) \
	{ \
		canvas_type_ copy_hd; \
		copy_hd.resolution = canvas_sd.resolution * resolution_factor; \
		copy_hd.grid = malloc(copy_hd.resolution * copy_hd.resolution * sizeof(element_type_)); \
		for (unsigned int y_hd = 0; y_hd < copy_hd.resolution; y_hd++) \
		for (unsigned int x_hd = 0; x_hd < copy_hd.resolution; x_hd++) \
		{ \
			const unsigned int x_sd = x_hd / resolution_factor; \
			const unsigned int y_sd = y_hd / resolution_factor; \
			const element_type_ element_sd = canvas_sd.grid[x_sd + canvas_sd.resolution * y_sd]; \
			copy_hd.grid[x_hd + copy_hd.resolution * y_hd] = element_sd; \
		} \
		return copy_hd; \
	} \
	\
	canvas_type_ function_prefix_##_copy_downscale(canvas_type_ canvas_hd, \
		unsigned int inverse_resolution_factor) \
	{ \
		canvas_type_ copy_sd; \
		copy_sd.resolution = canvas_hd.resolution / inverse_resolution_factor; \
		copy_sd.grid = malloc(copy_sd.resolution * copy_sd.resolution * sizeof(element_type_)); \
		for (unsigned int y_sd = 0; y_sd < copy_sd.resolution; y_sd++) \
		for (unsigned int x_sd = 0; x_sd < copy_sd.resolution; x_sd++) \
		{ \
			element_type_ element_sd = {0}; \
			for (unsigned int y_hd = y_sd * inverse_resolution_factor; \
				y_hd < (y_sd+1) * inverse_resolution_factor; y_hd++) \
			for (unsigned int x_hd = x_sd * inverse_resolution_factor; \
				x_hd < (x_sd+1) * inverse_resolution_factor; x_hd++) \
			{ \
				const element_type_ element_hd = \
					canvas_hd.grid[x_hd + canvas_hd.resolution * y_hd]; \
				element_sd = element_type_##_add(element_sd, element_hd); \
			} \
			const float normalization_factor = \
				inverse_resolution_factor * inverse_resolution_factor; \
			element_sd = element_type_##_div(element_sd, normalization_factor); \
			copy_sd.grid[x_sd + copy_sd.resolution * y_sd] = element_sd; \
		} \
		return copy_sd; \
	} \
	\
	canvas_type_ function_prefix_##_copy_expand(canvas_type_ canvas, \
		unsigned int new_resolution, element_type_ filling_element) \
	{ \
		assert(new_resolution >= canvas.resolution); \
		canvas_type_ copy; \
		copy.resolution = new_resolution; \
		copy.grid = malloc(copy.resolution * copy.resolution * sizeof(element_type_)); \
		for (unsigned int y = 0; y < canvas.resolution; y++) \
		{ \
			memcpy(&copy.grid[copy.resolution * y], &canvas.grid[canvas.resolution * y], \
				canvas.resolution * sizeof(element_type_)); \
			for (unsigned int x = canvas.resolution; x < copy.resolution; x++) \
			{ \
				copy.grid[x + copy.resolution * y] = filling_element; \
			} \
		} \
		for (unsigned int y = canvas.resolution; y < copy.resolution; y++) \
		for (unsigned int x = canvas.resolution; x < copy.resolution; x++) \
		{ \
			copy.grid[x + copy.resolution * y] = filling_element; \
		} \
		return copy; \
	} \
	\
	int function_prefix_##_is_in_bounds(canvas_type_ canvas, coords_t coords) \
	{ \
		const coords_grid_t coords_grid = coords_to_coords_grid(coords, canvas.resolution); \
		return INT_COORDS_IS_IN_BOUNDS(coords_grid, canvas.resolution); \
	} \
	\
	element_type_ function_prefix_##_get(canvas_type_ canvas, coords_t coords) \
	{ \
		const coords_grid_t coords_grid = coords_to_coords_grid(coords, canvas.resolution); \
		assert(INT_COORDS_IS_IN_BOUNDS(coords_grid, canvas.resolution)); \
		return canvas.grid[coords_grid.x + canvas.resolution * coords_grid.y]; \
	} \
	\
	void function_prefix_##_set(canvas_type_ canvas, coords_t coords, element_type_ color) \
	{ \
		const coords_grid_t coords_grid = coords_to_coords_grid(coords, canvas.resolution); \
		assert(INT_COORDS_IS_IN_BOUNDS(coords_grid, canvas.resolution)); \
		canvas.grid[coords_grid.x + canvas.resolution * coords_grid.y] = color; \
	} \
	\
	_Static_assert(1, "")

DEFINE_CANVAS(canvas_gs_op_t, canvas_gs_op, gs_op_t);
DEFINE_CANVAS(canvas_float_t, canvas_float, float);

canvas_gs_op_t canvas_float_to_gs_op(canvas_float_t canvas)
{
	canvas_gs_op_t copy = canvas_gs_op_init(canvas.resolution);
	for (unsigned int i = 0; i < copy.resolution * copy.resolution; i++)
	{
		copy.grid[i] = (gs_op_t){.gs = canvas.grid[i], .op = 1.0f};
	}
	return copy;
}

canvas_float_t canvas_gs_op_to_float(canvas_gs_op_t canvas, float background_gs)
{
	canvas_float_t copy = canvas_float_init(canvas.resolution);
	for (unsigned int i = 0; i < copy.resolution * copy.resolution; i++)
	{
		copy.grid[i] = gs_op_combine_background(background_gs, canvas.grid[i]);
	}
	return copy;
}

/* Plotter used by the canvas_gs_op_draw_line function. */
static void canvas_gs_op_draw_line_plotter(void* plot_data, pixel_line_t pixel)
{
	canvas_gs_op_t canvas = *(canvas_gs_op_t*)plot_data;
	if (canvas_gs_op_is_in_bounds(canvas, pixel.coords))
	{
		const gs_op_t bottom = canvas_gs_op_get(canvas, pixel.coords);
		canvas_gs_op_set(canvas, pixel.coords, gs_op_combine(bottom, pixel.color));
	}
}
void canvas_gs_op_draw_line_coords(canvas_gs_op_t canvas, line_coords_t line,
	line_plot_algorithm_t line_algorithm)
{
	line_algorithm(canvas_gs_op_draw_line_plotter, &canvas, line, canvas.resolution);
}

void canvas_gs_op_draw_line_pixels(canvas_gs_op_t canvas, line_pixels_t line)
{
	assert(canvas.resolution == line.resolution);
	for (unsigned int i = 0; i < line.pixel_count; i++)
	{
		pixel_line_t pixel = line.pixel_array[i];
		if (canvas_gs_op_is_in_bounds(canvas, pixel.coords))
		{
			const gs_op_t bottom = canvas_gs_op_get(canvas, pixel.coords);
			canvas_gs_op_set(canvas, pixel.coords, gs_op_combine(bottom, pixel.color));
		}
	}
}

/* Plotter used by the canvas_float_draw_line function. */
static void canvas_float_draw_line_plotter(void* plot_data, pixel_line_t pixel)
{
	canvas_float_t canvas = *(canvas_float_t*)plot_data;
	if (canvas_float_is_in_bounds(canvas, pixel.coords))
	{
		const float bottom = canvas_float_get(canvas, pixel.coords);
		canvas_float_set(canvas, pixel.coords, gs_op_combine_background(bottom, pixel.color));
	}
}
void canvas_float_draw_line_coords(canvas_float_t canvas, line_coords_t line,
	line_plot_algorithm_t line_algorithm)
{
	line_algorithm(canvas_float_draw_line_plotter, &canvas, line, canvas.resolution);
}

void canvas_float_draw_line_pixels(canvas_float_t canvas, line_pixels_t line)
{
	assert(canvas.resolution == line.resolution);
	for (unsigned int i = 0; i < line.pixel_count; i++)
	{
		pixel_line_t pixel = line.pixel_array[i];
		if (canvas_float_is_in_bounds(canvas, pixel.coords))
		{
			const float bottom = canvas_float_get(canvas, pixel.coords);
			canvas_float_set(canvas, pixel.coords, gs_op_combine_background(bottom, pixel.color));
		}
	}
}

/* Code factorisation to avoid duplication.
 * See the canvas_gs_op_output_bmp and canvas_float_output_bmp functions. */
#define CANVAS_OUTPUT_BMP(local_variable_declarations_, visible_gs_expression_) \
	do \
	{ \
		bmp_pixel_grid_t bmp_pixel_gird = {.w = canvas.resolution, .h = canvas.resolution}; \
		bmp_pixel_gird.grid = malloc(bmp_pixel_gird.w * bmp_pixel_gird.h * sizeof(bmp_pixel_t)); \
		for (unsigned int y = 0; y < canvas.resolution; y++) \
		for (unsigned int x = 0; x < canvas.resolution; x++) \
		{ \
			local_variable_declarations_; \
			const float visible_gs = visible_gs_expression_; \
			const unsigned char value = (1.0f - visible_gs) * 255.0f; \
			bmp_pixel_gird.grid[x + canvas.resolution * y] = \
				(bmp_pixel_t){value, value, value, 255}; \
		} \
		\
		output_bmp(bmp_pixel_gird, output_file_path); \
		free(bmp_pixel_gird.grid); \
	} while (0)

void canvas_gs_op_output_bmp(canvas_gs_op_t canvas, float background_gs,
	const char* output_file_path)
{
	/* TODO: Remove the now useless local_variable_declarations_ argument
	 * from CANVAS_OUTPUT_BMP. */
	CANVAS_OUTPUT_BMP(gs_op_t color, (
		color = canvas.grid[x + canvas.resolution * y],
		gs_op_combine_background(background_gs, color)
	));
}

void canvas_float_output_bmp(canvas_float_t canvas, const char* output_file_path)
{
	CANVAS_OUTPUT_BMP((void)0, (
		canvas.grid[x + canvas.resolution * y]
	));
}
