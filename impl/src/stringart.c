
#include "stringart.h"
#include "basics.h"
#include "line.h"
#include "canvas.h"
#include "pinset.h"
#include "random.h"
#include "utils.h"
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
 * on the given current canvas, in comparison to the given target canvas.
 * Through out_positive_error_delta, the sum of the positive error deltas can be returned too. */
static float error_delta_line(canvas_float_t target_canvas, canvas_gs_op_t current_canvas,
	float current_canvas_background_gs,
	line_pixels_t line, error_formula_t error_formula,
	float* out_positive_error_delta)
{
	assert(target_canvas.resolution == line.resolution);
	assert(current_canvas.resolution == line.resolution);
	const unsigned int resolution = line.resolution;

	float error_delta_acc = 0.0f;
	float positive_error_delta_acc = 0.0f;
	for (unsigned int i = 0; i < line.pixel_count; i++)
	{
		const pixel_line_t pixel_line = line.pixel_array[i];
		const coords_grid_t coords_grid = coords_to_coords_grid(pixel_line.coords, resolution);
		const float target_gs = target_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		const gs_op_t old_color = current_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		const float old_gs = gs_op_combine_background(current_canvas_background_gs, old_color);
		const float new_gs = gs_op_combine_background(old_gs, pixel_line.color);
		const float error_delta_value = error_delta(target_gs, old_gs, new_gs, error_formula);
		error_delta_acc += error_delta_value;
		positive_error_delta_acc += error_delta_value > 0.0f ? error_delta_value : 0.0f;
	}
	if (out_positive_error_delta != NULL)
	{
		*out_positive_error_delta = positive_error_delta_acc;
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

/* Returns the average grayscale color of the target canvas pixels
 * that would be covered by drawing the given line. */
static float average_gs_target_line(canvas_float_t target_canvas, line_pixels_t line,
	canvas_float_t importance_canvas)
{
	assert(target_canvas.resolution == line.resolution);
	assert(importance_canvas.resolution == line.resolution);
	const unsigned int resolution = line.resolution;

	float gs_acc = 0.0f;
	float importance_sum = 0.0f;
	for (unsigned int i = 0; i < line.pixel_count; i++) 
	{
		coords_grid_t coords_grid = coords_to_coords_grid(line.pixel_array[i].coords, resolution);
		float importance = importance_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		gs_acc += importance * target_canvas.grid[coords_grid.x + resolution * coords_grid.y];
		importance_sum += importance;
	}
	return gs_acc / importance_sum;
}

void perform_string_art(string_art_input_t input)
{
	assert(input.resolution_factor > 0);
	const unsigned int resolution_sd = input.input_canvas.resolution;
	const unsigned int resolution_hd = input.input_canvas.resolution * input.resolution_factor;
	const float correction_factor = input.resolution_factor * input.resolution_factor;
	
	canvas_float_t importance_canvas = input.importance_canvas;
	canvas_float_t target_canvas = input.input_canvas;
	canvas_float_t target_canvas_hd =
		canvas_float_copy_upscale(input.input_canvas, input.resolution_factor);
	canvas_float_t target_erase_canvas = input.input_canvas;
	canvas_gs_op_t current_canvas_sd = canvas_gs_op_init_fill(resolution_sd, (gs_op_t){.op = 0.0f});
	canvas_gs_op_t current_canvas_hd = canvas_gs_op_init_fill(resolution_hd, (gs_op_t){.op = 0.0f});
	const float current_canvas_background_gs = input.current_canvas_background_gs;
	const error_formula_t error_formula =
		input.error_formula == ERROR_FORMULA_DIFF ? error_formula_diff :
		input.error_formula == ERROR_FORMULA_DIFF_SQUARE ? error_formula_diff_square :
		(assert(0), (error_formula_t)NULL);
	pinset_t pinset = input.pinset;
	const unsigned int iteration_max_number = input.iteration_max_number;

	line_stats_t* line_stats_da = NULL;
	unsigned int line_stats_len = 0;
	unsigned int line_stats_cap = 0;
	
	iteration_stats_t* iteration_stats_da = NULL;
	unsigned int iteration_stats_len = 0;
	unsigned int iteration_stats_cap = 0;

	canvas_float_output_bmp(target_canvas, "A_target.bmp");
	canvas_float_output_bmp(importance_canvas, "A_importance.bmp");

	rg_t rg;
	rg_time_seed(&rg);
	
	const unsigned int line_pool_length = input.line_pool_length;
	assert(line_pool_length >= 2);
	line_coords_t line_pool[line_pool_length];

	const unsigned int line_number_per_iteration = input.line_number_per_iteration;
	assert(line_number_per_iteration >= 1);
	assert(line_number_per_iteration <= line_pool_length);
	float best_score_array[line_number_per_iteration];
	int best_line_index_array[line_number_per_iteration];
	line_stats_t best_line_stats_array[line_number_per_iteration];

	unsigned int line_i = 0;

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

		for (unsigned int i = 0; i < line_number_per_iteration; i++)
		{
			best_score_array[i] = -FLT_MAX;
			best_line_index_array[i] = -1;
		}

		for (unsigned int i = 0; i < line_pool_length; i++)
		{
			line_pixels_t line_pixels_hd = line_coords_to_line_pixels(
				line_pool[i],
				resolution_hd,
				line_plot_pixels_mid_point);
			line_pixels_t line_pixels_sd = line_pixels_downscale_hd_to_sd(line_pixels_hd,
				resolution_sd);

			line_stats_t line_stats = {0};

			line_stats.error_delta_sd = error_delta_line(target_canvas,
				current_canvas_sd, current_canvas_background_gs,
				line_pixels_sd, error_formula, &line_stats.positive_error_delta_sd);

			float positive_error_delta_hd;
			const float error_delta_hd = error_delta_line(target_canvas_hd,
				current_canvas_hd, current_canvas_background_gs,
				line_pixels_hd, error_formula, &positive_error_delta_hd);
			line_stats.positive_error_delta_hd_corrected =
				positive_error_delta_hd / correction_factor;
			line_stats.error_delta_hd_corrected =
				error_delta_hd / correction_factor;

			line_stats.delta_sd = error_delta_line_self(current_canvas_sd,
				current_canvas_background_gs,
				line_pixels_sd, error_formula);

			const float delta_hd = error_delta_line_self(current_canvas_hd,
				current_canvas_background_gs,
				line_pixels_hd, error_formula);
			line_stats.delta_hd_corrected =
				delta_hd / correction_factor;

			line_stats.avg_gs_erase_target =
				average_gs_target_line(target_erase_canvas, line_pixels_sd, importance_canvas);

			line_stats.avg_gs_target =
				average_gs_target_line(target_canvas, line_pixels_sd, importance_canvas);

			line_pixels_cleanup(line_pixels_hd);
			line_pixels_cleanup(line_pixels_sd);

			switch (input.score_formula)
			{
				case SCORE_FORMULA_DIFF_AVG_GS_ERASE_TARGET:
					line_stats.score = 
						fabsf(line_stats.avg_gs_erase_target - current_canvas_background_gs);
				break;
				case SCORE_FORMULA_DIFF_AVG_GS_TARGET:
					line_stats.score =
						fabsf(line_stats.avg_gs_target - current_canvas_background_gs);
				break;
				case SCORE_FORMULA_NEG_ERROR_DELTA:
					line_stats.score = -line_stats.error_delta_sd;
				break;
				case SCORE_FORMULA_NEG_ERROR_DELTA_PLUS_DELTA_SD:
					line_stats.score = -line_stats.error_delta_sd + line_stats.delta_sd / 4.0f;
				break;
				case SCORE_FORMULA_NEG_ERROR_DELTA_IF_DELTA_SD:
					line_stats.score =
						line_stats.delta_sd > 0.001f ?
							-line_stats.error_delta_sd : -line_stats.delta_sd;
				break;
				case SCORE_FORMULA_NEG_ERROR_DELTA_NORMALIZED:
					line_stats.score =
						-line_stats.error_delta_sd / (float)line_pixels_sd.pixel_count;
				break;
				case SCORE_FORMULA_NEG_ERROR_DELTA_POSITIVE_ERROR_PENALTY:
					line_stats.score =
						-line_stats.error_delta_sd
						-line_stats.positive_error_delta_hd_corrected * 0.8f;
				break;
				case SCORE_FORMULA_DIFF_AVG_GS_ERASE_TARGET_POSITIVE_ERROR_PENALTY:
					line_stats.score = 
						fabsf(line_stats.avg_gs_erase_target - current_canvas_background_gs)
						-line_stats.positive_error_delta_hd_corrected * 0.8f;
				break;
			}

			unsigned int j = 0;
			while (j < line_number_per_iteration && line_stats.score > best_score_array[j])
			{
				if (j > 0)
				{
					best_score_array[j-1] = best_score_array[j];
					best_line_index_array[j-1] = best_line_index_array[j];
					best_line_stats_array[j-1] = best_line_stats_array[j];
				}
				j++;
			}
			if (j > 0)
			{
				best_score_array[j-1] = line_stats.score;
				best_line_index_array[j-1] = i;
				best_line_stats_array[j-1] = line_stats;
			}
		}
		for (unsigned int i = 0; i < line_number_per_iteration; i++)
		{
			assert(best_line_index_array[i] != -1);
		}

		unsigned int line_index_lower = line_i;
		unsigned int line_number = 0;
		for (unsigned int i = 0; i < line_number_per_iteration; i++)
		{
			const float score = best_score_array[i];
			if (score <= 0.0f)
			{
				continue;
			}

			DA_LENGTHEN(line_stats_len += 1, line_stats_cap, line_stats_da, line_stats_t);
			assert(line_i == line_stats_len-1);
			line_stats_da[line_i] = best_line_stats_array[i];

			const line_coords_t line = line_pool[best_line_index_array[i]];

			canvas_gs_op_draw_line_coords(current_canvas_hd, line, line_plot_pixels_mid_point);
			canvas_gs_op_draw_line_coords(current_canvas_sd, line, line_plot_pixels_mid_point);

			line_coords_t line_neg = line;
			/* TODO: FIXME */
			line_neg.color.gs = current_canvas_background_gs *
				fabsf(line.color.gs - current_canvas_background_gs);
			canvas_float_draw_line_coords(target_erase_canvas,
				line_neg, line_plot_pixels_mid_point);

			if (line_i % 100 == 0 && line_i != 0)
			{
				char file_name[99];

				sprintf(file_name, "line_%04d.bmp", line_i);
				canvas_gs_op_output_bmp(current_canvas_hd, current_canvas_background_gs, file_name);

				sprintf(file_name, "line_erase_%04d.bmp", line_i);
				canvas_float_output_bmp(target_erase_canvas, file_name);
			}

			if (line_i % 100 == 0 && line_i > 0)
			{
				#if 0
				printf("%.5d %.5d   % 6.4f % 6.4f % 6.4f   "
					"% 11.5f % 11.5f  % 11.5f % 11.5f  % 11.5f\n",
					iteration_i, line_i,
					best_score_array[0], best_score_array[line_number_per_iteration-1],
					best_score_array[line_number_per_iteration-1] - best_score_array[0],
					best_line_error_delta_array[0],
					best_line_error_delta_array[line_number_per_iteration-1],
					best_line_positive_error_delta_hd_array[line_number_per_iteration-1],
					best_line_delta_sd_array[line_number_per_iteration-1]);
				#endif
			}

			line_i++;
			line_number++;
		}

		if (iteration_i % 3 == 0)
		{
			#if 0
			printf("%.5d %.5d   % 6.4f % 6.4f % 6.4f   "
				"% 11.5f % 11.5f  % 11.5f % 11.5f  % 11.5f\n",
				iteration_i, line_i,
				best_score_array[0], best_score_array[line_number_per_iteration-1],
				best_score_array[line_number_per_iteration-1] - best_score_array[0],
				best_line_error_delta_array[0],
				best_line_error_delta_array[line_number_per_iteration-1],
				best_line_positive_error_delta_hd_array[line_number_per_iteration-1],
				best_line_delta_sd_array[line_number_per_iteration-1]);
			#endif
			printf("%.5d\n", line_i);
		}

		DA_LENGTHEN(iteration_stats_len += 1, iteration_stats_cap,
			iteration_stats_da, iteration_stats_t);
		assert(iteration_i == iteration_stats_len-1);
		iteration_stats_da[iteration_i].line_index_lower = line_index_lower;
		iteration_stats_da[iteration_i].line_number = line_number;
	}

	canvas_float_output_bmp(target_erase_canvas, "A_target_erase.bmp");
	canvas_gs_op_output_bmp(current_canvas_hd, current_canvas_background_gs, "A_hd.bmp");
	canvas_gs_op_output_bmp(current_canvas_sd, current_canvas_background_gs, "A_sd.bmp");
}
