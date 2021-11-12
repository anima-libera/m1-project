
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
			.final_line_number_max = 1024 * 8,
			.line_data_pool_len_max = 512,
			.line_pool_generator = linepoolgen_random,
			.line_score_computer = linescorecomp_error_sq_new,
			.winning_line_handler = winlinehand_draw_erase_log_print,
			.halt_on_average_grayscale_crossing = 0,
			.halt_on_average_grayscale_hd_crossing = 1,
			.halt_on_error_stagnation_length = 0,//100,
		},
		.input = {
			.input = input,
			.line_colorset = line_colorset,
			.line_colorset_len =
				(sizeof line_colorset) / (sizeof line_colorset[0]),
			.pinset = {.w = input.w, .h = input.h, .pin_number = pin_number},
			.hd_sd_ratio = 3,
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
	output_pg_as_bitmap(mem.state.canvas_hd, "output_hd.bmp");
	output_pg_as_bitmap(mem.state.target, "target.bmp");
	output_pg_as_bitmap(mem.logdata.trace, "trace.bmp");

	FILE* data_log_file = fopen("data_log", "w");

	fprintf(data_log_file, "%f\n",
		mem.logdata.input_average_grayscale);
	
	fprintf(data_log_file, "\n");
	for (unsigned int i = 0; i < mem.state.iteration; i++)
	{
		fprintf(data_log_file, "%f %f %f %f %f %f %f %f %f\n",
			mem.logdata.error_delta_array[i],
			mem.logdata.error_new_array[i],
			mem.logdata.error_sq_delta_array[i],
			mem.logdata.error_sq_new_array[i],
			mem.logdata.line_minimal_radius_array[i],
			mem.logdata.average_grayscale_array[i],
			mem.logdata.average_grayscale_hd_array[i],
			mem.logdata.error_cavnas_input_array[i],
			mem.logdata.error_sq_cavnas_input_array[i]);
	}

	fclose(data_log_file);

	return 0;
}
