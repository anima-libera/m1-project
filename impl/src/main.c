
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "plotter.h"
#include "utils.h"
#include "pinset.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

int main(int argc, const char** argv)
{
	/* Parse command line arguments. */
	const char* output_file_path = "out.bmp";
	for (unsigned int i = 1; i < (unsigned int)argc; i++)
	{
		if (strcmp(argv[i], "-o") == 0)
		{
			i++;
			assert(i < (unsigned int)argc);
			output_file_path = argv[i];
		}
	}

	pg_t target;
	pg_init_1024_white_disc(&target, 0);
	for (unsigned int y = 0; y < target.h; y++)
	for (unsigned int x = 0; x < target.w; x++)
	{
		if (dist(x, y, target.w/2 - 200, target.h/2) <= target.w/5)
		{
			target.pixel_grid[x + target.w * y] = (pixel_t){
				.r = 0,
				.g = 0,
				.b = 150,
				.a = 255,
			};
		}
	}

	pg_t pg;
	pg_init_1024_white_disc(&pg, 255);
	pinset_t pinset = {.w = pg.w, .h = pg.h, .pin_number = 128};

	#if 0
	int pin_index_a = 0;
	float xa, ya;
	while (pinset_get_pin_pos(pinset, pin_index_a++, &xa, &ya))
	{
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

			float error = 0.0f;
			const float max_raw_pixel_error = 255.0f * 3.0f;
			for (unsigned int i = 0; i < pm_da.len; i++)
			{
				pm_t pm = pm_da.arr[i];
				error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].r) / max_raw_pixel_error;
				error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].g) / max_raw_pixel_error;
				error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].b) / max_raw_pixel_error;
			}
			if (pm_da.len != 0)
			{
				error /= (float)pm_da.len;
			}

			if (error < min_error)
			{
				min_error = error;
				min_error_pin_index = pin_index_b;
			}
			free(pm_da.arr);
			pin_index_b++;
		}

		pinset_get_pin_pos(pinset, min_error_pin_index, &xb, &yb);
		line_mid_point(plotter_plot, &pg,
			(pixel_t){0, 0, 255, 255},
			xa, ya, xb, yb);
	}
	#endif

	/* Keep that vvvvv !!!!! */
	#if 0
	int pin_index = 0;
	float xa, ya, xb, yb;
	pinset_get_pin_pos(pinset, pin_index++, &xa, &ya);
	while (pinset_get_pin_pos(pinset, pin_index++, &xb, &yb))
	{
		float min_error = 9999.0f;
		int min_error_pin_index = -1;

		pm_da_t pm_da = {0};
		line_mid_point(plotter_pm_da, &pm_da,
			(pixel_t){0, 0, 255, 255},
			xa, ya, xb, yb);

		if (pm_da.len < 100)
		{
			line_mid_point(plotter_plot, &pg,
				(pixel_t){0, 255, 0, 255},
				xa, ya, xb, yb);
			continue;
		}

		float error = 0.0f;
		const float max_raw_pixel_error = 255.0f * 1.0f;// 3.0f;
		for (unsigned int i = 0; i < pm_da.len; i++)
		{
			pm_t pm = pm_da.arr[i];
			//error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].r) / max_raw_pixel_error;
			//error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].g) / max_raw_pixel_error;
			error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].b) / max_raw_pixel_error;
		}
		error /= (float)pm_da.len;

		printf("0 - %d error: %f\n", pin_index, error);

		if (error < min_error)
		{
			min_error = error;
			min_error_pin_index = pin_index;
		}
		free(pm_da.arr);
		pin_index++;

		unsigned int mark_a = error * 255.0f;
		unsigned int mark_b = clamp(error * 10.0f - 9.0f, 0.0f, 1.0f) * 255.0f;
		unsigned int mark_c = clamp(error * 100.0f - 99.0f, 0.0f, 1.0f) * 255.0f;
		
		line_mid_point(plotter_plot, &pg,
			(pixel_t){mark_a, mark_b, mark_c, 255},
			xa, ya, xb, yb);
	}
	#endif

	#if 1
	int pin_index_a = 0;
	float xa, ya;
	while (pinset_get_pin_pos(pinset, pin_index_a++, &xa, &ya))
	{
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
			for (unsigned int i = 0; i < pm_da.len; i++)
			{
				pm_t pm = pm_da.arr[i];
				error += fabsf(pm.r - target.pixel_grid[pm.x + target.w * pm.y].r) / max_raw_pixel_error;
				error += fabsf(pm.g - target.pixel_grid[pm.x + target.w * pm.y].g) / max_raw_pixel_error;
				error += fabsf(pm.b - target.pixel_grid[pm.x + target.w * pm.y].b) / max_raw_pixel_error;
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
		line_mid_point(plotter_plot, &pg,
			(pixel_t){0, 0, 255, 255},
			xa, ya, xb, yb);
	}
	#endif

	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
