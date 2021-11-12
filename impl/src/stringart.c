
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

static float pg_error_in_disc(pg_t target, pg_t current, int square)
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
			float pixel_error_value = pixel_error(pix_tar, pix_cur);
			error += pixel_error_value * (square ? pixel_error_value : 1.0f);
			pixel_count++;
		}
	}
	return error / (float)pixel_count;
}

static float line_minimal_radius(stringart_mem_t* mem, const line_data_t* line_data)
{
	float x_line_mid = (line_data->line.xa + line_data->line.xb) / 2.0f;
	float y_line_mid = (line_data->line.ya + line_data->line.yb) / 2.0f;
	float x_center = (float)mem->state.canvas.w / 2.0f;
	float y_center = (float)mem->state.canvas.h / 2.0f;
	float radius = dist(x_line_mid, y_line_mid, x_center, y_center);
	return radius;
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
	pg_init_white(&mem->state.canvas_hd,
		mem->input.input.w * mem->input.hd_sd_ratio, mem->input.input.h * mem->input.hd_sd_ratio);
	pg_init_copy(&mem->state.target, mem->input.input);
	pg_init_copy(&mem->logdata.trace, mem->state.canvas);
	mem->logdata.input_average_grayscale =
		pg_average_grayscale_in_disc(mem->input.input);
	mem->logdata.error_delta_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_new_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_sq_delta_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_sq_new_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.line_minimal_radius_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.average_grayscale_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.average_grayscale_hd_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_cavnas_input_array =
		malloc(mem->algo.final_line_number_max * sizeof(float));
	mem->logdata.error_sq_cavnas_input_array =
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
		
		float winning_line_minimal_radius =
			line_minimal_radius(mem, winning_line_data);
		float canvas_average_grayscale =
			pg_average_grayscale_in_disc(mem->state.canvas);
		float canvas_hd_average_grayscale =
			pg_average_grayscale_in_disc(mem->state.canvas_hd);
		float error_cavnas_input =
			pg_error_in_disc(mem->input.input, mem->state.canvas, 0);
		float error_sq_cavnas_input =
			pg_error_in_disc(mem->input.input, mem->state.canvas, 1);

		printf("%5d / %d  % .5f %.5f % .5f %.5f  %6.1f   %.5f|%.5f / %.5f   %.5f %.5f\n",
			mem->state.iteration+1,
			mem->algo.final_line_number_max,
			winning_line_data->error_delta,
			winning_line_data->error_new,
			winning_line_data->error_sq_delta,
			winning_line_data->error_sq_new,
			winning_line_minimal_radius,
			canvas_average_grayscale,
			canvas_hd_average_grayscale,
			mem->logdata.input_average_grayscale,
			error_cavnas_input,
			error_sq_cavnas_input);

		mem->logdata.error_delta_array[mem->state.iteration] =
			winning_line_data->error_delta;
		mem->logdata.error_new_array[mem->state.iteration] =
			winning_line_data->error_new;
		mem->logdata.error_sq_delta_array[mem->state.iteration] =
			winning_line_data->error_sq_delta;
		mem->logdata.error_sq_new_array[mem->state.iteration] =
			winning_line_data->error_sq_new;
		mem->logdata.line_minimal_radius_array[mem->state.iteration] =
			winning_line_minimal_radius;
		mem->logdata.average_grayscale_array[mem->state.iteration] =
			canvas_average_grayscale;
		mem->logdata.average_grayscale_hd_array[mem->state.iteration] =
			canvas_hd_average_grayscale;
		mem->logdata.error_cavnas_input_array[mem->state.iteration] =
			error_cavnas_input;
		mem->logdata.error_sq_cavnas_input_array[mem->state.iteration] =
			error_sq_cavnas_input;

		if (mem->algo.halt_on_average_grayscale_crossing &&
			canvas_average_grayscale > mem->logdata.input_average_grayscale)
		{
			printf("Halting: average grayscale crossing.\n");
			running = 0;
		}

		if (mem->algo.halt_on_average_grayscale_hd_crossing &&
			canvas_hd_average_grayscale > mem->logdata.input_average_grayscale)
		{
			printf("Halting: average grayscale HD crossing.\n");
			running = 0;
		}

		if (mem->algo.halt_on_error_stagnation_length != 0 &&
			mem->state.iteration > mem->algo.halt_on_error_stagnation_length)
		{
			float old_error =
				mem->logdata.error_cavnas_input_array[
					mem->state.iteration - mem->algo.halt_on_error_stagnation_length];
			float error_delta = error_cavnas_input - old_error;
			if (error_delta > -0.000001f)
			{
				printf("Halting: error stagnation.\n");
				running = 0;
			}
		}

		mem->state.iteration++;
		if (mem->state.iteration >= mem->algo.final_line_number_max)
		{
			printf("Halting: line limit reached.\n");
			running = 0;
		}
	}
}

static void line_pm_da_hd_to_sd(stringart_mem_t* mem,
	pm_da_t pm_da_hd, pm_da_t* pm_da_sd)
{
	for (unsigned int i = 0; i < pm_da_hd.len; i++)
	{
		pm_t pm_hd = pm_da_hd.arr[i];

		unsigned int x_sd = pm_hd.x / mem->input.hd_sd_ratio;
		unsigned int y_sd = pm_hd.y / mem->input.hd_sd_ratio;

		pm_t* pm_sd = NULL;
		int j;
		for (j = pm_da_sd->len-1; j > 0; j--)
		{
			if (pm_da_sd->arr[j].x == x_sd && pm_da_sd->arr[j].y == y_sd)
			{
				pm_sd = &pm_da_sd->arr[j];
				break;
			}
		}
		if (pm_sd == NULL)
		{
			DA_LENGTHEN(++pm_da_sd->len, pm_da_sd->cap, pm_da_sd->arr, pm_t);
			pm_sd = &pm_da_sd->arr[pm_da_sd->len-1];
			*pm_sd = (pm_t){.x = x_sd, .y = y_sd, .a = 0};
		}

		float sd_pixel_hd_size = mem->input.hd_sd_ratio * mem->input.hd_sd_ratio;
		float alpha_total = pm_sd->a + (float)pm_hd.a / sd_pixel_hd_size;
		float alpha_ratio = (float)pm_sd->a / alpha_total;
		
		#define CHANNEL(c_) \
			pm_sd->c_ = (float)pm_sd->c_ * alpha_ratio + (float)pm_hd.c_ * (1.0f - alpha_ratio)
		CHANNEL(r);
		CHANNEL(g);
		CHANNEL(b);
		#undef CHANNEL
		pm_sd->a += (float)pm_hd.a / sd_pixel_hd_size;
	}
}

static void line_sd_to_hd(stringart_mem_t* mem, line_t line_sd, line_t* line_hd)
{
	*line_hd = line_sd;
	line_hd->xa *= (float)mem->input.hd_sd_ratio;
	line_hd->ya *= (float)mem->input.hd_sd_ratio;
	line_hd->xb *= (float)mem->input.hd_sd_ratio;
	line_hd->yb *= (float)mem->input.hd_sd_ratio;
}

static void line_data_compute_delta_and_new(stringart_mem_t* mem,
	line_data_t* line_data)
{
	line_t line_sd = line_data->line;
	line_t line_hd;
	line_sd_to_hd(mem, line_sd, &line_hd);

	pm_da_t pm_da_hd = {0};
	line_mid_point(plotter_pm_da, &pm_da_hd,
		line_hd);

	pm_da_t pm_da_sd = {0};
	line_pm_da_hd_to_sd(mem, pm_da_hd, &pm_da_sd);

	line_data->error_delta = 0.0f;
	line_data->error_new = 0.0f;
	line_data->error_sq_delta = 0.0f;
	line_data->error_sq_new = 0.0f;
	for (unsigned int k = 0; k < pm_da_sd.len; k++)
	{
		pm_t pm = pm_da_sd.arr[k];
		pg_t canvas = mem->state.canvas;
		pg_t target = mem->state.target;

		uint8_t pm_r = pm.r * pm.a / 255;
		uint8_t pm_g = pm.g * pm.a / 255;
		uint8_t pm_b = pm.b * pm.a / 255;

		pixel_t cur_pix = canvas.pixel_grid[pm.x + canvas.w * pm.y];
		pixel_t tar_pix = target.pixel_grid[pm.x + target.w * pm.y];
		pixel_t new_pix = {pm_r, pm_g, pm_b, 255};

		float pix_old_error = pixel_error(tar_pix, cur_pix);
		float pix_new_error = pixel_error(tar_pix, new_pix);
		float pix_error_delta = pix_new_error - pix_old_error;
		float pix_old_error_sq = pix_old_error * pix_old_error;
		float pix_new_error_sq = pix_new_error * pix_new_error;
		float pix_error_sq_delta = pix_new_error_sq - pix_old_error_sq;

		line_data->error_delta += pix_error_delta;
		line_data->error_new += pix_new_error;
		line_data->error_sq_delta += pix_error_sq_delta;
		line_data->error_sq_new += pix_new_error_sq;
	}
	line_data->error_delta /= (float)pm_da_sd.len;
	line_data->error_new /= (float)pm_da_sd.len;
	line_data->error_sq_delta /= (float)pm_da_sd.len;
	line_data->error_sq_new /= (float)pm_da_sd.len;

	free(pm_da_hd.arr);
	free(pm_da_sd.arr);
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
		line_data_compute_delta_and_new(mem, &line_data_pool[i]);
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
		line_data_compute_delta_and_new(mem, &line_data_pool[i]);
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

float linescorecomp_error_sq_new(stringart_mem_t* mem,
	line_data_t* line_data)
{
	(void)mem;
	return -line_data->error_sq_new;
}

float linescorecomp_error_sq_delta(stringart_mem_t* mem,
	line_data_t* line_data)
{
	(void)mem;
	return -line_data->error_sq_delta;
}

static void plot_pm_da(pg_t pg, pm_da_t pm_da)
{
	for (unsigned int i = 0; i < pm_da.len; i++)
	{
		pm_t pm = pm_da.arr[i];
		if (pm.x == pg.w)
		{
			pm.x--;
		}
		if (pm.y == pg.h)
		{
			pm.y--;
		}
		#if 0
		if (pg.w <= pm.x || pg.h <= pm.y)
		{
			fprintf(stderr, "(access of (%d, %d) is outside of range (0~%d, 0~%d))\n",
				pm.x, pm.y, pg.w, pg.h);
			continue;
		}
		#endif
		pixel_t* pixel = &pg.pixel_grid[pm.x + pg.w * pm.y];

		float alpha_total = pixel->a + (float)pm.a;
		float alpha_ratio = (float)pixel->a / alpha_total;
		
		#define CHANNEL(c_) \
			pixel->c_ = (float)pixel->c_ * alpha_ratio + (float)pm.c_ * (1.0f - alpha_ratio)
		CHANNEL(r);
		CHANNEL(g);
		CHANNEL(b);
		#undef CHANNEL
		pixel->a += pm.a;
	}
}

static void erase_pm_da(pg_t pg, pm_da_t pm_da)
{
	for (unsigned int i = 0; i < pm_da.len; i++)
	{
		pm_t pm = pm_da.arr[i];
		if (pm.x == pg.w)
		{
			pm.x--;
		}
		if (pm.y == pg.h)
		{
			pm.y--;
		}
		#if 0
		if (pg.w <= pm.x || pg.h <= pm.y)
		{
			fprintf(stderr, "(access of (%d, %d) is outside of range (0~%d, 0~%d))\n",
				pm.x, pm.y, pg.w, pg.h);
			continue;
		}
		#endif
		pixel_t* pixel = &pg.pixel_grid[pm.x + pg.w * pm.y];

		#define CHANNEL(c_) \
			pixel->c_ = \
				pixel->c_ * (1.0f - (float)pm.a / 255.0f) + \
				(255.0f - (float)pm.c_) * (float)pm.a / 255.0f
		CHANNEL(r);
		CHANNEL(g);
		CHANNEL(b);
		#undef CHANNEL
	}
}

static int winlinehand_generic(stringart_mem_t* mem,
	line_data_t* line_data, int erase_on_target)
{
	line_t line_sd = line_data->line;
	line_t line_hd;
	line_sd_to_hd(mem, line_sd, &line_hd);

	pm_da_t pm_da_hd = {0};
	line_mid_point(plotter_pm_da, &pm_da_hd,
		line_hd);
	plot_pm_da(mem->state.canvas_hd, pm_da_hd);

	pm_da_t pm_da_sd = {0};
	line_pm_da_hd_to_sd(mem, pm_da_hd, &pm_da_sd);
	plot_pm_da(mem->state.canvas, pm_da_sd);

	if (erase_on_target)
	{
		erase_pm_da(mem->state.target, pm_da_sd);
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
