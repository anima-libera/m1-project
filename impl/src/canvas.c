
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
	canvas_type_ function_prefix_##_init_fill(unsigned int resolution, \
		element_type_ filling_elem) \
	{ \
		canvas_type_ canvas = function_prefix_##_init(resolution); \
		for (unsigned int i = 0; i < resolution * resolution; i++) \
		{ \
			canvas.grid[i] = filling_elem; \
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
		#if 0
		pixel_line_t pixel = line.pixel_array[i];
		coords_grid_t coords_grid = coords_to_coords_grid(pixel.coords, canvas.resolution);
		gs_op_t* dst_color = &canvas.grid[coords_grid.x + canvas.resolution * coords_grid.y];
		*dst_color = gs_op_combine(*dst_color, pixel.color);
		#endif
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
