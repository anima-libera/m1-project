
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "plotter.h"
#include "utils.h"
#include "pinset.h"
#include "random.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>

#if 0
struct line_t
{
	float xa, xb, ya, yb;
	pixel_t color;
};
typedef struct line_t line_t;
#endif

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

void pg_init_blue_circles(pg_t* pg)
{
	pg_init_1024_white(pg);
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		if (dist(x, y, pg->w/2 - 200, pg->h/2) <= pg->w/7)
		{
			pg->pixel_grid[x + pg->w * y] = (pixel_t){
				.r = 0,
				.g = 0,
				.b = 255,
				.a = 255,
			};
		}
		else if (dist(x, y, pg->w/2 - 150, pg->h/2 - 200) <= pg->w/8)
		{
			pg->pixel_grid[x + pg->w * y] = (pixel_t){
				.r = 0,
				.g = 0,
				.b = 255,
				.a = 255,
			};
		}
		else if (dist(x, y, pg->w/2, pg->h/2 - 250) <= pg->w/18)
		{
			pg->pixel_grid[x + pg->w * y] = (pixel_t){
				.r = 0,
				.g = 0,
				.b = 255,
				.a = 255,
			};
		}
	}
}

void pg_init_bird(pg_t* pg)
{
	pg->w = 1704;
	pg->h = 2272;
	unsigned int buffer_size = pg->w * pg->h * sizeof(pixel_t);
	pg->pixel_grid = malloc(buffer_size);
	FILE* bird_raw_file = fopen("../rawpics/bird.raw", "rb");
	fread(pg->pixel_grid, buffer_size, 1, bird_raw_file);
	fclose(bird_raw_file);
}

int main(int argc, const char** argv)
{
	/* Parse command line arguments. */
	const char* input_file_path = "input.bmp";
	const char* output_file_path = "output.bmp";
	const char* target_file_path = "target.bmp";
	const char* trace_file_path = "trace.bmp";
	const char* error_file_path = "error";
	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i++], "-i") == 0 ||
			strcmp(argv[i++], "--input") == 0)
		{
			assert(i < (unsigned int)argc);
			input_file_path = argv[i];
		}
		else if (strcmp(argv[i++], "-o") == 0 ||
			strcmp(argv[i++], "--output") == 0)
		{
			assert(i < (unsigned int)argc);
			output_file_path = argv[i];
		}
		else if (strcmp(argv[i++], "--target") == 0)
		{
			assert(i < (unsigned int)argc);
			target_file_path = argv[i];
		}
		else if (strcmp(argv[i++], "--trace") == 0)
		{
			assert(i < (unsigned int)argc);
			trace_file_path = argv[i];
		}
		else if (strcmp(argv[i++], "--error") == 0)
		{
			assert(i < (unsigned int)argc);
			error_file_path = argv[i];
		}
	}

	pg_t input;
	pg_init_blue_circles(&input);
	//pg_init_bird(&input);

	pg_t target;
	pg_init_copy(&target, input);

	pg_t canvas;
	pg_init_white(&canvas, input.w, input.h);

	pg_t trace;
	pg_init_copy(&trace, canvas);

	pinset_t pinset = {.w = canvas.w, .h = canvas.h, .pin_number = 1024};

	rg_t rg;
	rg_time_seed(&rg);

	unsigned int line_number = 512;
	float error_array[line_number];

	clock_t clock_start = clock();

	string_art_01(pinset, canvas, target, trace, &rg,
		error_array, line_number);

	clock_t clock_end = clock();
	float time_taken =
		(float)(clock_end - clock_start) / (float)CLOCKS_PER_SEC;
	printf("Time taken: %f sec\n", time_taken);

	output_pg_as_bitmap(input, input_file_path);
	output_pg_as_bitmap(canvas, output_file_path);
	output_pg_as_bitmap(target, target_file_path);
	output_pg_as_bitmap(trace, trace_file_path);

	FILE* error_file = fopen(error_file_path, "w");
	for (unsigned int i = 0; i < line_number; i++)
	{
		fprintf(error_file, "%f\n", error_array[i]);
	}
	fclose(error_file);

	return 0;
}
