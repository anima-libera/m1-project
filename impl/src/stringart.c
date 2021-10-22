
#include "stringart.h"
#include "pg.h"
#include "pinset.h"
#include "line.h"
#include "plotter.h"
#include "random.h"
#include "utils.h"
#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <assert.h>
#include <float.h>

#define LINE_RECOLORED(line_, color_) \
	(line_t){ \
		.color = (color_), \
		.xa = (line_).xa, .ya = (line_).ya, \
		.xb = (line_).xb, .yb = (line_).yb \
	}

static float pixel_error(pixel_t target, pixel_t current)
{
	float channel_error_sum =
		fabsf(target.r - current.r) / 255.0f +
		fabsf(target.g - current.g) / 255.0f +
		fabsf(target.b - current.b) / 255.0f;
	return channel_error_sum / 3.0f;
}

static float pg_error_in_disc(pg_t target, pg_t current)
{
	float error = 0.0f;
	unsigned int pixel_count = 0;
	for (unsigned int y = 0; y < target.h; y++)
	for (unsigned int x = 0; x < target.w; x++)
	{
		if (dist(x, y, target.w/2, target.h/2) <= target.w/2)
		{
			pixel_t pix_cur = current.pixel_grid[x + current.w * y];
			pixel_t pix_tar = target.pixel_grid[x + target.w * y];
			error += pixel_error(pix_tar, pix_cur);
			pixel_count++;
		}
	}
	return error / (float)pixel_count;
}

void stringart_perform_algo(stringart_mem_t* mem)
{
	mem->state.iteration = 0;
	if (mem->input.rg_seed == 0)
	{
		rg_time_seed(&mem->state.rg);
	}
	else
	{
		rg_seed(&mem->state.rg, mem->input.rg_seed);
	}
	pg_init_white(&mem->state.canvas,
		mem->input.input.w, mem->input.input.h);
	pg_init_copy(&mem->state.target, mem->input.input);
	pg_init_copy(&mem->logdata.trace, mem->state.canvas);
	mem->logdata.input_average_grayscale =
		pg_average_grayscale_in_disc(mem->input.input);
	mem->logdata.error_delta_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_new_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.average_grayscale_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_cavnas_input_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	
	line_data_t line_data_pool[mem->algo.line_data_pool_len_max];
	int running = 1;
	while (running)
	{
		/* Generate a line pool using `algo.line_pool_generator`. */
		unsigned int line_data_pool_len = mem->algo.line_pool_generator(mem,
			line_data_pool, mem->algo.line_data_pool_len_max);
		
		/* Find the best line using `algo.line_score_computer`. */
		float best_score = -FLT_MAX;
		int best_line_index = -1;
		for (unsigned int i = 0; i < line_data_pool_len; i++)
		{
			float score = mem->algo.line_score_computer(mem,
				&line_data_pool[i]);
			
			if (score > best_score)
			{
				best_score = score;
				best_line_index = i;
			}
		}
		assert(best_line_index != -1);
		line_data_t* winning_line_data = &line_data_pool[best_line_index];

		/* Handle this best line, i.e. call `algo.winning_line_handler`. */
		running = mem->algo.winning_line_handler(mem, winning_line_data);

		float completion =
			(float)mem->state.iteration /
			(float)mem->algo.final_line_number_max;
		line_mid_point(plotter_plot, &mem->logdata.trace,
			LINE_RECOLORED(winning_line_data->line, ((pixel_t){
				completion * 255.0f,
				0,
				(1.0f - completion) * 255.0f,
				255
			})));
		
		float canvas_average_grayscale =
			pg_average_grayscale_in_disc(mem->state.canvas);
		float error_cavnas_input =
			pg_error_in_disc(mem->input.input, mem->state.canvas);

		printf("%5d / %d  % .5f  %.5f   %.5f / %.5f   %.5f\n",
			mem->state.iteration+1,
			mem->algo.final_line_number_max,
			winning_line_data->error_delta,
			winning_line_data->error_new,
			canvas_average_grayscale,
			mem->logdata.input_average_grayscale,
			error_cavnas_input);

		mem->logdata.error_delta_array[mem->state.iteration] =
			winning_line_data->error_delta;
		mem->logdata.error_new_array[mem->state.iteration] =
			winning_line_data->error_new;
		mem->logdata.average_grayscale_array[mem->state.iteration] =
			canvas_average_grayscale;
		mem->logdata.error_cavnas_input_array[mem->state.iteration] =
			error_cavnas_input;

		if (mem->algo.halt_on_average_grayscale_crossing &&
			canvas_average_grayscale > mem->logdata.input_average_grayscale)
		{
			running = 0;
		}

		mem->state.iteration++;
		if (mem->state.iteration >= mem->algo.final_line_number_max)
		{
			running = 0;
		}
	}
}

static void line_dta_compute_delta_and_new(stringart_mem_t* mem,
	line_data_t* line_data)
{
	pm_da_t pm_da = {0};
	line_mid_point(plotter_pm_da, &pm_da,
		line_data->line);

	line_data->error_delta = 0.0f;
	line_data->error_new = 0.0f;
	for (unsigned int k = 0; k < pm_da.len; k++)
	{
		pm_t pm = pm_da.arr[k];
		pg_t canvas = mem->state.canvas;
		pg_t target = mem->state.target;

		assert(pm.a == 255);

		pixel_t cur_pix = canvas.pixel_grid[pm.x + canvas.w * pm.y];
		pixel_t tar_pix = target.pixel_grid[pm.x + target.w * pm.y];
		pixel_t new_pix = {pm.r, pm.g, pm.b, pm.a};

		float pix_old_error = pixel_error(tar_pix, cur_pix);
		float pix_new_error = pixel_error(tar_pix, new_pix);
		float pix_error_delta = pix_new_error - pix_old_error;

		line_data->error_delta += pix_error_delta;
		line_data->error_new += pix_new_error;
	}
	line_data->error_delta /= (float)pm_da.len;
	line_data->error_new /= (float)pm_da.len;

	free(pm_da.arr);
}

unsigned int linepoolgen_random(stringart_mem_t* mem,
	line_data_t* line_data_pool, unsigned int line_data_pool_len)
{
	for (unsigned int i = 0; i < line_data_pool_len; i++)
	{
		line_t line;
		unsigned int color_index = rg_int(&mem->state.rg,
			0, mem->input.line_colorset_len-1);
		line.color = mem->input.line_colorset[color_index];
		int pin_a = rg_int(&mem->state.rg,
			0, mem->input.pinset.pin_number-1);
		pinset_get_pin_pos(mem->input.pinset, pin_a, &line.xa, &line.ya);
		int pin_b;
		while (1)
		{
			pin_b = rg_int(&mem->state.rg,
				0, mem->input.pinset.pin_number-1);
			pinset_get_pin_pos(mem->input.pinset, pin_b, &line.xb, &line.yb);
			if (dist(line.xa, line.ya, line.xb, line.yb) > 100.0f)
			{
				break;
			}
		}
		line_data_pool[i].line = line;
		line_data_pool[i].pin_a = pin_a;
		line_data_pool[i].pin_b = pin_b;
		line_dta_compute_delta_and_new(mem, &line_data_pool[i]);
	}
	return line_data_pool_len;
}

unsigned int linepoolgen_iter_random(stringart_mem_t* mem,
	line_data_t* line_data_pool, unsigned int line_data_pool_len)
{
	#define NUMBER_OF_TURNS 5
	int pin_a =
		(float)mem->state.iteration /
		(float)mem->algo.final_line_number_max *
		(float)mem->input.pinset.pin_number *
		(float)NUMBER_OF_TURNS;
	for (unsigned int i = 0; i < line_data_pool_len; i++)
	{
		line_t line;
		unsigned int color_index = rg_int(&mem->state.rg,
			0, mem->input.line_colorset_len-1);
		line.color = mem->input.line_colorset[color_index];
		pinset_get_pin_pos(mem->input.pinset, pin_a, &line.xa, &line.ya);
		int pin_b;
		while (1)
		{
			pin_b = rg_int(&mem->state.rg,
				0, mem->input.pinset.pin_number-1);
			pinset_get_pin_pos(mem->input.pinset, pin_b, &line.xb, &line.yb);
			if (dist(line.xa, line.ya, line.xb, line.yb) > 100.0f)
			{
				break;
			}
		}
		line_data_pool[i].line = line;
		line_data_pool[i].pin_a = pin_a;
		line_data_pool[i].pin_b = pin_b;
		line_dta_compute_delta_and_new(mem, &line_data_pool[i]);
	}
	return line_data_pool_len;
}

float linescorecomp_error_new(stringart_mem_t* mem,
	line_data_t* line_data)
{
	(void)mem;
	return -line_data->error_new;
}

float linescorecomp_error_delta(stringart_mem_t* mem,
	line_data_t* line_data)
{
	(void)mem;
	return -line_data->error_delta;
}

static int winlinehand_generic(stringart_mem_t* mem,
	line_data_t* line_data, int erase_on_target)
{
	line_t line = line_data->line;

	line_mid_point(plotter_plot, &mem->state.canvas,
		line);

	if (erase_on_target)
	{
		line_mid_point(plotter_plot, &mem->state.target,
			LINE_RECOLORED(line_data->line,
				((pixel_t){255, 255, 255, 255})));
	}

	return 1;
}

int winlinehand_draw_erase_log_print(stringart_mem_t* mem,
	line_data_t* line_data)
{
	return winlinehand_generic(mem, line_data, 1);
}

int winlinehand_draw_log_print(stringart_mem_t* mem,
	line_data_t* line_data)
{
	return winlinehand_generic(mem, line_data, 0);
}
