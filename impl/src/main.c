
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
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int main(int argc, const char** argv)
{
	/* Parse command line arguments. */
	const char* input_pic_name = NULL;
	const char* output_directory_path = NULL;
	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-i") == 0 ||
			strcmp(argv[i], "--input-pic-name") == 0)
		{
			i++;
			assert(i < (unsigned int)argc);
			input_pic_name = argv[i];
		}
		else if (strcmp(argv[i], "-o") == 0 ||
			strcmp(argv[i], "--output-directory-path") == 0)
		{
			i++;
			assert(i < (unsigned int)argc);
			output_directory_path = argv[i];
		}
		else if (argv[i][0] != '-')
		{
			if (input_pic_name == NULL)
			{
				input_pic_name = argv[i];
			}
			else if (output_directory_path == NULL)
			{
				output_directory_path = argv[i];
			}
		}
	}

	if (input_pic_name)
	{
		printf("Input picture name: %s\n", input_pic_name);
	}
	else
	{
		printf("No input picture name\n");
	}

	if (output_directory_path == NULL)
	{
		output_directory_path = "out";
	}
	printf("Output directory path: %s\n", output_directory_path);
	
	{
		struct stat od_stat = {0};
		if (stat(output_directory_path, &od_stat) == -1)
		{
			int execl(const char* path, const char* arg, ...);
			execl("rm", "-r", output_directory_path);
		}
		//if (mkdir(output_directory_path, S_IRWXU | S_IRWXG | S_IRWXO) == -1)
		//{
		//	printf("Couldn't create the output directory %s\n", output_directory_path);
		//}
	}

	pg_t input;
	if (input_pic_name == NULL)
	{
		printf("Using a default input picture\n");
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
			.final_line_number_max = 120,//1024 * 8,
			.line_data_pool_len_max = 30,
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
			.line_color_target_average = 0,
			.pinset = {.w = input.w, .h = input.h, .pin_number = pin_number},
			.hd_sd_ratio = 1,
			.rg_seed = 0, /* Uses timestamp. */
			.output_every_n_iterations = 100,
			.output_directory_path = output_directory_path,
		}
	};

	clock_t clock_start = clock();

	stringart_perform_algo(&mem);

	clock_t clock_end = clock();
	float time_taken =
		(float)(clock_end - clock_start) / (float)CLOCKS_PER_SEC;
	printf("Done in %f sec\n", time_taken);
	
	#define OUTPUT_PG(pg_, bmp_file_name_) \
		do \
		{ \
			char* file_path = filepath_join(output_directory_path, bmp_file_name_); \
			output_pg_as_bitmap(pg_, file_path); \
			free(file_path); \
		} \
		while (0)
	
	OUTPUT_PG(mem.input.input, "input.bmp");
	OUTPUT_PG(mem.state.canvas, "output.bmp");
	OUTPUT_PG(mem.state.canvas_hd, "output_hd.bmp");
	OUTPUT_PG(mem.state.target, "target.bmp");
	OUTPUT_PG(mem.logdata.trace, "trace.bmp");

	#undef OUTPUT_PG

	char* file_path = filepath_join(output_directory_path, "data_log");
	FILE* data_log_file = fopen(file_path, "w");
	free(file_path);

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

	FILE* last_output_directory_file = fopen("last_output_directory", "w");
	fprintf(last_output_directory_file, "%s\n", output_directory_path);
	fclose(last_output_directory_file);

	return 0;
}
