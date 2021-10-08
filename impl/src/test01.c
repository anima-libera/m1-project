
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "plotter.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

pixel_t compute_pm_color(pixel_t current, pm_t pm)
{
	float pm_opacity = (float)pm.a / 255.0f;
	return (pixel_t){
		.r = (float)current.r * (1.0f - pm_opacity) + (float)pm.r * pm_opacity,
		.g = (float)current.g * (1.0f - pm_opacity) + (float)pm.g * pm_opacity,
		.b = (float)current.b * (1.0f - pm_opacity) + (float)pm.b * pm_opacity,
		.a = 255, /* What to put here ? */
	};
}

float compute_error_color_to_color(pixel_t target, pixel_t current)
{
	return
		fabsf((float)target.r / 255.0f - (float)current.r / 255.0f) +
		fabsf((float)target.g / 255.0f - (float)current.g / 255.0f) +
		fabsf((float)target.b / 255.0f - (float)current.b / 255.0f);
}

float compute_error_delta_pixel(pixel_t target, pixel_t current, pm_t pm)
{
	float initial_error = compute_error_color_to_color(target, current);
	pixel_t new_color = compute_pm_color(current, pm);
	float new_error = compute_error_color_to_color(target, new_color);
	return new_error - initial_error;
}

float compute_error_delta_pm_da(pg_t target, pg_t current, pm_da_t pm_da)
{
	float error_delta_sum = 0.0f;
	for (unsigned int i = 0; i < pm_da.len; i++)
	{
		pm_t pm = pm_da.arr[i];
		pixel_t target_color = target.pixel_grid[pm.x + target.w * pm.y];
		pixel_t current_color = current.pixel_grid[pm.x + current.w * pm.y];

		error_delta_sum += compute_error_delta_pixel(target_color, current_color, pm);
	}
	return error_delta_sum;
}

struct line_t
{
	float xa, xb, ya, yb;
	pixel_t color;
};
typedef struct line_t line_t;

float compute_error_delta_line(pg_t target, pg_t current, const line_t* line)
{
	pm_da_t pm_da = {0};
	line_xiaolin_wu(plotter_pm_da, &pm_da,
		line->color, line->xa, line->ya, line->xb, line->yb);
	float error_delta = compute_error_delta_pm_da(target, current, pm_da);
	free(pm_da.arr);
	return error_delta;
}

void mutate_line_color(line_t* line)
{
	line->color.r += rand() % 31 - 15;
	line->color.g += rand() % 31 - 15;
	line->color.b += rand() % 31 - 15;
}

void mutate_line_position(line_t* line, float pg_w, float pg_h)
{
	line->xa += (float)(rand() % 31 - 15);
	line->ya += (float)(rand() % 31 - 15);
	line->xb += (float)(rand() % 31 - 15);
	line->yb += (float)(rand() % 31 - 15);
	line->xa = clamp(line->xa, 0.0f, pg_w);
	line->ya = clamp(line->ya, 0.0f, pg_h);
	line->xb = clamp(line->xb, 0.0f, pg_w);
	line->yb = clamp(line->yb, 0.0f, pg_h);
}

void mutate_line(line_t* line, float pg_w, float pg_h)
{
	if (rand() % 3 == 0)
	{
		mutate_line_color(line);
	}
	else
	{
		mutate_line_position(line, pg_w, pg_h);
	}
}

void proceed_to_test01(void)
{
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

	pg_t current;
	pg_init_1024_white_disc(&current, 255);

	pg_t trace;
	pg_init_1024_white_disc(&trace, 255);

	for (unsigned int j = 0; j < 20; j++)
	{
		line_t best_line = {
			.color = {255, 0, 0, 255},
			.xa = 300.0f, .ya = 300.0f, .xb = 700.0f, .yb = 700.0f,
		};

		for (unsigned int i = 0; i < 8000; i++)
		{
			line_t line = best_line;
			mutate_line(&line, current.w, current.h);
			float error_delta = compute_error_delta_line(target, current, &line);
			if (error_delta < -200.0f)//0.0f)
			{
				best_line = line;
				printf("(%d) %f\n", i, error_delta);
				line_xiaolin_wu(plotter_plot, &trace,
					best_line.color,
					best_line.xa, best_line.ya, best_line.xb, best_line.yb);
			}
			else
			{
				printf(".");
			}
		}

		line_xiaolin_wu(plotter_plot, &current,
			best_line.color,
			best_line.xa, best_line.ya, best_line.xb, best_line.yb);
		printf("%d\n", j);
	}

	output_pg_as_bitmap(target, "target.bmp");
	output_pg_as_bitmap(current, "result.bmp");
	output_pg_as_bitmap(trace, "trace.bmp");
}
