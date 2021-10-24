
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "plotter.h"
#include "utils.h"
#include "pinset.h"
#include "random.h"
#include "stringart.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <float.h> /* FLT_MAX */
#include <limits.h>



#if 0
int choose_pin_a(pinset_t pinset, rg_t* rg,
	unsigned int line_i, unsigned int line_number, unsigned int j)
{
	(void)line_i;
	(void)line_number;
	return rg_int(rg, 0, pinset.pin_number-1);
}

int choose_pin_b(pinset_t pinset, rg_t* rg,
	unsigned int line_i, unsigned int line_number, unsigned int j,
	int pin_a)
{
	(void)line_i;
	(void)line_number;
	float xa, ya, xb, yb;
	pinset_get_pin_pos(pinset, pin_a, &xa, &ya);
	while (1)
	{
		int pin_b = rg_int(rg, 0, pinset.pin_number-1);
		pinset_get_pin_pos(pinset, pin_b, &xb, &yb);
		if (dist(xa, ya, xb, yb) > 100.0f)
		{
			return pin_b;
		}
	}
}
#elif 0
int choose_pin_a(pinset_t pinset, rg_t* rg,
	unsigned int i, unsigned int line_number, unsigned int j)
{
	(void)j;
	return (float)i * (float)pinset.pin_number / (float)line_number;
}

int choose_pin_b(pinset_t pinset, rg_t* rg,
	unsigned int i, unsigned int line_number, unsigned int j,
	int pin_a)
{
	(void)i;
	(void)line_number;
	(void)j;
	float xa, ya, xb, yb;
	pinset_get_pin_pos(pinset, pin_a, &xa, &ya);
	while (1)
	{
		int pin_b = rg_int(rg, 0, pinset.pin_number-1);
		pinset_get_pin_pos(pinset, pin_b, &xb, &yb);
		if (dist(xa, ya, xb, yb) > 100.0f)
		{
			return pin_b;
		}
	}
}
#elif 0
int choose_pin_a(pinset_t pinset, rg_t* rg,
	unsigned int i, unsigned int line_number, unsigned int j)
{
	(void)i;
	(void)line_number;
	(void)j;
	return rg_int(rg, 0, pinset.pin_number-1);
}

int choose_pin_b(pinset_t pinset, rg_t* rg,
	unsigned int i, unsigned int line_number, unsigned int j,
	int pin_a)
{
	(void)i;
	return (float)j * (float)pinset.pin_number / (float)line_number;
}
#endif

#if 0
void string_art_02(pinset_t pinset, pg_t canvas, pg_t target, pg_t trace,
	rg_t* rg, unsigned int line_number,
	float* error_delta_array, float* error_new_array,
	float* average_grayscale_array)
{
	assert(rg != NULL);

	float input_average_grayscale = pg_average_grayscale_in_disc(target);

	for (unsigned int i = 0; i < line_number; i++)
	{
		line_t best_line;
		float best_error_delta = FLT_MAX;
		float best_error_new = FLT_MAX;

		for (unsigned int j = 0; j < 1000; j++)
		{
			/* Generate random line. */

			line_t line = {.color = {0, 0, 0, 255}};
			int pin_a = choose_pin_a(pinset, rg, i, line_number, j);
			int pin_b = choose_pin_b(pinset, rg, i, line_number, j, pin_a);
			pinset_get_pin_pos(pinset, pin_a, &line.xa, &line.ya);
			pinset_get_pin_pos(pinset, pin_b, &line.xb, &line.yb);

			/* Test the generated line. */

			pm_da_t pm_da = {0};
			line_mid_point(plotter_pm_da, &pm_da,
				line.color, line.xa, line.ya, line.xb, line.yb);

			float error_delta = 0.0f;
			float error_new = 0.0f;
			for (unsigned int k = 0; k < pm_da.len; k++)
			{
				pm_t pm = pm_da.arr[k];
				assert(pm.a == 255);
				pixel_t cur_pix = canvas.pixel_grid[pm.x + canvas.w * pm.y];
				pixel_t tar_pix = target.pixel_grid[pm.x + target.w * pm.y];
				pixel_t new_pix = {pm.r, pm.g, pm.b, pm.a};

				float pix_old_error = pixel_error(tar_pix, cur_pix);
				float pix_new_error = pixel_error(tar_pix, new_pix);
				float pix_error_delta = pix_new_error - pix_old_error;

				error_delta += pix_error_delta;
				error_new += pix_new_error;
			}
			error_delta /= (float)pm_da.len;
			error_new /= (float)pm_da.len;

			free(pm_da.arr);

			if (error_new < best_error_new)
			{
				best_error_delta = error_delta;
				best_error_new = error_new;
				best_line = line;
			}
		}

		assert(best_error_delta < FLT_MAX);

		line_mid_point(plotter_plot, &canvas,
			best_line.color,
			best_line.xa, best_line.ya, best_line.xb, best_line.yb);

		line_mid_point(plotter_plot, &target,
			(pixel_t){255, 255, 255, 255},
			best_line.xa, best_line.ya, best_line.xb, best_line.yb);

		line_mid_point(plotter_plot, &trace,
			(pixel_t){(float)i / (float)line_number * 255.0f, 0, 0, 255},
			best_line.xa, best_line.ya, best_line.xb, best_line.yb);
		
		float canvas_average_grayscale = pg_average_grayscale_in_disc(canvas);

		printf("%5d / %d  % .5f  %.5f   %.5f vs %.5f\n",
			i+1, line_number, best_error_delta, best_error_new,
			canvas_average_grayscale, input_average_grayscale);
		if (error_delta_array != NULL)
		{
			error_delta_array[i] = best_error_delta;
		}
		if (error_new_array != NULL)
		{
			error_new_array[i] = best_error_new;
		}
		if (average_grayscale_array != NULL)
		{
			average_grayscale_array[i] = canvas_average_grayscale;
		}

		#if 1
		if (canvas_average_grayscale > input_average_grayscale)
		{
			break;
		}
		#endif
	}
}
#endif

#if 0
void string_art_01(pinset_t pinset, pg_t canvas, pg_t target, pg_t trace,
	rg_t* rg, float* error_array, unsigned int line_number)
{
	assert(rg != NULL);
	float xa, ya;
	for (unsigned int i = 0; i < line_number; i++)
	{
		int pin_index_a = rg_int(rg, 0, pinset.pin_number-1);
		pinset_get_pin_pos(pinset, pin_index_a, &xa, &ya);

		float min_error = 9999.0f;
		int min_error_pin_index = -1;
		int pin_index_b = 0;
		float xb, yb;
		while (pinset_get_pin_pos(pinset, pin_index_b, &xb, &yb))
		{
			pm_da_t pm_da = {0};
			line_mid_point(plotter_pm_da, &pm_da,
				(pixel_t){0, 0, 255, 255},
				xa, ya, xb, yb);

			if (pm_da.len < 100)
			{
				pin_index_b++;
				continue;
			}

			float error = 0.0f;
			const float max_raw_pixel_error = 255.0f * 3.0f;
			for (unsigned int j = 0; j < pm_da.len; j++)
			{
				pm_t pm = pm_da.arr[j];
				pixel_t tar_pix = target.pixel_grid[pm.x + target.w * pm.y];
				error += fabsf(pm.r - tar_pix.r) / max_raw_pixel_error;
				error += fabsf(pm.g - tar_pix.g) / max_raw_pixel_error;
				error += fabsf(pm.b - tar_pix.b) / max_raw_pixel_error;
			}
			error /= (float)pm_da.len;

			if (error < min_error)
			{
				min_error = error;
				min_error_pin_index = pin_index_b;
			}
			free(pm_da.arr);
			pin_index_b++;
		}

		pinset_get_pin_pos(pinset, min_error_pin_index, &xb, &yb);
		line_mid_point(plotter_plot, &canvas,
			(pixel_t){0, 0, 255, 255},
			xa, ya, xb, yb);
		line_mid_point(plotter_plot, &target,
			(pixel_t){255, 255, 255, 255},
			xa, ya, xb, yb);
		line_mid_point(plotter_plot, &trace,
			(pixel_t){(float)i / (float)line_number * 255.0f, 0, 0, 255},
			xa, ya, xb, yb);
		
		printf("%5d / %d  %.5f\n", i+1, line_number, min_error);
		if (error_array != NULL)
		{
			error_array[i] = min_error;
		}
	}
}
#endif

int main(int argc, const char** argv)
{
	/* Parse command line arguments. */
	const char* input_pic_name = NULL;
	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-i") == 0 ||
			strcmp(argv[i], "--input-pic-name") == 0)
		{
			i++;
			assert(i < (unsigned int)argc);
			input_pic_name = argv[i];
		}
		else if (argv[i][0] != '-')
		{
			if (input_pic_name == NULL)
			{
				input_pic_name = argv[i];
			}
		}
	}

	pg_t input;
	if (input_pic_name == NULL)
	{
		pg_init_circles(&input, (pixel_t){0, 0, 0, 255});
	}
	else
	{
		pg_init_pic_name(&input, input_pic_name);
		pg_grayscalize(input);
	}

	pixel_t line_colorset[] = {
		{0, 0, 0, 255}, 
		//{128, 128, 128, 255}
	};

	unsigned int pin_number = 1024;

	stringart_mem_t mem = {
		.algo = {
			.final_line_number_max = 1024 * 3,
			.line_data_pool_len_max = 512,
			.line_pool_generator = linepoolgen_random,
			.line_score_computer = linescorecomp_error_new,
			.winning_line_handler = winlinehand_draw_erase_log_print,
			.halt_on_average_grayscale_crossing = 0,
		},
		.input = {
			.input = input,
			.line_colorset = line_colorset,
			.line_colorset_len =
				(sizeof line_colorset) / (sizeof line_colorset[0]),
			.pinset = {.w = input.w, .h = input.h, .pin_number = pin_number},
			.rg_seed = 0, /* Uses timestamp. */
		}
	};

	clock_t clock_start = clock();

	stringart_perform_algo(&mem);

	clock_t clock_end = clock();
	float time_taken =
		(float)(clock_end - clock_start) / (float)CLOCKS_PER_SEC;
	printf("Done in %f sec\n", time_taken);

	output_pg_as_bitmap(mem.input.input, "input.bmp");
	output_pg_as_bitmap(mem.state.canvas, "output.bmp");
	output_pg_as_bitmap(mem.state.target, "target.bmp");
	output_pg_as_bitmap(mem.logdata.trace, "trace.bmp");

	FILE* data_log_file = fopen("data_log", "w");

	fprintf(data_log_file, "%f\n",
		mem.logdata.input_average_grayscale);
	
	fprintf(data_log_file, "\n");
	for (unsigned int i = 0; i < mem.state.iteration; i++)
	{
		fprintf(data_log_file, "%f %f %f %f\n",
			mem.logdata.error_delta_array[i],
			mem.logdata.error_new_array[i],
			mem.logdata.average_grayscale_array[i],
			mem.logdata.error_cavnas_input_array[i]);
	}

	fclose(data_log_file);

	#if 0
	pg_t target;
	pg_init_copy(&target, input);

	pg_t canvas;
	pg_init_white(&canvas, input.w, input.h);

	pg_t trace;
	pg_init_copy(&trace, canvas);

	pinset_t pinset = {.w = canvas.w, .h = canvas.h, .pin_number = 1024};

	rg_t rg;
	rg_time_seed(&rg);

	unsigned int line_number = 5000;
	float error_delta_array[line_number];
	float error_new_array[line_number];
	float average_grayscale_array[line_number];

	clock_t clock_start = clock();

	//string_art_01(pinset, canvas, target, trace, &rg,
	//	error_delta_array, line_number);
	string_art_02(pinset, canvas, target, trace, &rg, line_number,
		error_delta_array, error_new_array,
		average_grayscale_array);

	clock_t clock_end = clock();
	float time_taken =
		(float)(clock_end - clock_start) / (float)CLOCKS_PER_SEC;
	printf("Time taken: %f sec\n", time_taken);

	const char* gen_input_file_path = "input.bmp";
	const char* gen_output_file_path = "output.bmp";
	const char* gen_target_file_path = "target.bmp";
	const char* gen_trace_file_path = "trace.bmp";
	const char* gen_data_log_file_path = "data_log";

	output_pg_as_bitmap(input, gen_input_file_path);
	output_pg_as_bitmap(canvas, gen_output_file_path);
	output_pg_as_bitmap(target, gen_target_file_path);
	output_pg_as_bitmap(trace, gen_trace_file_path);

	FILE* data_log_file = fopen(gen_data_log_file_path, "w");

	float input_average_grayscale = pg_average_grayscale_in_disc(input);

	fprintf(data_log_file, "%f\n",
		input_average_grayscale);
	fprintf(data_log_file, "\n");
	for (unsigned int i = 0; i < line_number; i++)
	{
		fprintf(data_log_file, "%f %f %f\n",
			error_delta_array[i], error_new_array[i],
			average_grayscale_array[i]);
	}

	fclose(data_log_file);
	#endif

	return 0;
}
