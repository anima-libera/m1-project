
#include "stringart.h"
#include "basics.h"
#include "line.h"
#include "canvas.h"
#include "pinset.h"
#include "random.h"
#include <math.h>
#include <stdlib.h>
#include <assert.h>
#include <float.h>
#include <stdio.h>

/* Returns the error, between 0.0f and 1.0f, induced buy the given grayscale color,
 * in comparison to the given target color grayscale. */
typedef float (*error_formula_t)(float target_gs, float gs);

static float error_formula_diff(float target_gs, float gs)
{
	return fabsf(target_gs - gs);
}

static float error_formula_diff_square(float target_gs, float gs)
{
	const float diff = error_formula_diff(target_gs, gs);
	return diff * diff;
}

/* Returns the evolution of the error that would happen when replacing
 * the given old grayscale color by the given new one,
 * in comparison to the given target color grayscale.
 * As the error is between 0.0f and 1.0f, the delta is between -1.0f and 1.0f. */
static float error_delta(float target_gs, float old_gs, float new_gs,
	error_formula_t error_formula)
{
	const float old_error = error_formula(target_gs, old_gs);
	const float new_error = error_formula(target_gs, new_gs);
	return new_error - old_error;
}

/* Returns the sum of error deltas induced by drawing every pixels of the given line
 * on the given current canvas, in comparison to the given target canvas. */
static float error_delta_line(canvas_float_t target_canvas, canvas_gs_op_t current_canvas,
	float current_canvas_background_gs,
	line_pixels_t line, error_formula_t error_formula)
{
	assert(target_canvas.resolution == line.resolution);
	assert(current_canvas.resolution == line.resolution);
	const unsigned int resolution = line.resolution;

	float error_delta_acc = 0.0f;
	for (unsigned int i = 0; i < line.pixel_count; i++)
	{
		const pixel_line_t pixel_line = line.pixel_array[i];
		const coords_grid_t coords_grid = coords_to_coords_grid(pixel_line.coords, resolution);
		const float target_gs = target_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		const gs_op_t old_color = current_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		const float old_gs = gs_op_combine_background(current_canvas_background_gs, old_color);
		const float new_gs = gs_op_combine_background(old_gs, pixel_line.color);
		error_delta_acc += error_delta(target_gs, old_gs, new_gs, error_formula);
	}
	return error_delta_acc;
}

/* TODO: error_delta_line_self is a temporary workaround, make error_delta_line somehow generic */

static float error_delta_line_self(canvas_gs_op_t current_canvas,
	float current_canvas_background_gs,
	line_pixels_t line, error_formula_t error_formula)
{
	assert(current_canvas.resolution == line.resolution);
	const unsigned int resolution = line.resolution;

	float error_delta_acc = 0.0f;
	for (unsigned int i = 0; i < line.pixel_count; i++)
	{
		const pixel_line_t pixel_line = line.pixel_array[i];
		const coords_grid_t coords_grid = coords_to_coords_grid(pixel_line.coords, resolution);
		const gs_op_t old_color = current_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		const float old_gs = gs_op_combine_background(current_canvas_background_gs, old_color);
		const float new_gs = gs_op_combine_background(old_gs, pixel_line.color);
		error_delta_acc += error_delta(old_gs, old_gs, new_gs, error_formula);
	}
	return error_delta_acc;
}

void perform_string_art(string_art_input_t input)
{
	assert(input.resolution_factor > 0);
	const unsigned int resolution_sd = input.target_canvas.resolution;
	const unsigned int resolution_hd = input.target_canvas.resolution * input.resolution_factor;
	
	canvas_float_t target_canvas = input.target_canvas;
	canvas_gs_op_t current_canvas_sd = canvas_gs_op_init_fill(resolution_sd, (gs_op_t){.op = 0.0f});
	canvas_gs_op_t current_canvas_hd = canvas_gs_op_init_fill(resolution_hd, (gs_op_t){.op = 0.0f});
	const float current_canvas_background_gs = input.current_canvas_background_gs;
	const error_formula_t error_formula =
		input.error_formula == ERROR_FORMULA_DIFF ? error_formula_diff :
		input.error_formula == ERROR_FORMULA_DIFF_SQUARE ? error_formula_diff_square :
		(assert(0), (error_formula_t)NULL);
	pinset_t pinset = input.pinset;
	const unsigned int iteration_max_number = input.iteration_max_number;

	rg_t rg;
	rg_time_seed(&rg);
	
	const unsigned int line_pool_length = input.line_pool_length;
	assert(line_pool_length >= 2);
	line_coords_t line_pool[line_pool_length];

	for (unsigned int iteration_i = 0; iteration_i < iteration_max_number; iteration_i++)
	{
		for (unsigned int i = 0; i < line_pool_length; i++)
		{
			do
			{
				const unsigned int pin_index_a = rg_int(&rg, 0, pinset.pin_count - 1);
				const unsigned int pin_index_b = rg_int(&rg, 0, pinset.pin_count - 1);
				line_pool[i] = (line_coords_t){
					.coords_a = pinset.pin_array[pin_index_a],
					.coords_b = pinset.pin_array[pin_index_b],
					.color = input.line_color,
				};
			} while (line_coords_length(line_pool[i]) < 0.05f);
		}

		float best_score = -FLT_MAX;
		int best_line_index = -1;
		float best_line_error_delta;
		float best_line_delta_hd_corrected;
		float best_line_delta_sd;

		for (unsigned int i = 0; i < line_pool_length; i++)
		{
			line_pixels_t line_pixels_hd = line_coords_to_line_pixels(
				line_pool[i],
				resolution_hd,
				line_plot_pixels_mid_point);
			line_pixels_t line_pixels_sd = line_pixels_downscale_hd_to_sd(line_pixels_hd,
				resolution_sd);

			const float error_delta = error_delta_line(target_canvas,
				current_canvas_sd, current_canvas_background_gs,
				line_pixels_sd, error_formula);

			const float delta_hd = error_delta_line_self(current_canvas_hd,
				current_canvas_background_gs,
				line_pixels_hd, error_formula);
			const float delta_hd_corrected = delta_hd /
				(float)(input.resolution_factor * input.resolution_factor);

			const float delta_sd = error_delta_line_self(current_canvas_sd,
				current_canvas_background_gs,
				line_pixels_sd, error_formula);

			line_pixels_cleanup(line_pixels_hd);
			line_pixels_cleanup(line_pixels_sd);

			float score = -error_delta;// + delta_sd/3.0f;
			score /= (float)line_pixels_sd.pixel_count;

			if (score > best_score)
			{
				best_score = score;
				best_line_index = i;
				best_line_error_delta = error_delta;
				best_line_delta_hd_corrected = delta_hd_corrected;
				best_line_delta_sd = delta_sd;
			}
		}
		assert(best_line_index != -1);

		const line_coords_t best_line = line_pool[best_line_index];

		canvas_gs_op_draw_line_coords(current_canvas_hd, best_line, line_plot_pixels_mid_point);
		canvas_gs_op_draw_line_coords(current_canvas_sd, best_line, line_plot_pixels_mid_point);

		if (iteration_i % 100 == 0 && iteration_i != 0)
		{
			char file_name[99];
			sprintf(file_name, "step_%04d.bmp", iteration_i);
			canvas_gs_op_output_bmp(current_canvas_hd, 0.0f, file_name);
		}

		if (iteration_i % 20 == 0)
		{
			printf("%.5d % 10.5f % 10.5f % 10.5f % 10.5f\n",
				iteration_i,
				best_line_error_delta,
				best_line_delta_hd_corrected,
				best_line_delta_sd,
				best_line_error_delta != 0 ? best_line_delta_sd / -best_line_error_delta : 1.0f);
		}
	}

	canvas_gs_op_output_bmp(current_canvas_hd, 0.0f, "canvas_hd.bmp");
	canvas_gs_op_output_bmp(current_canvas_sd, 0.0f, "canvas_sd.bmp");
	canvas_float_output_bmp(target_canvas, "target.bmp");
}
