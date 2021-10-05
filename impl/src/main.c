
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>

void plotter_print(void* ptr,
	int x, int y, float brightness, pixel_t color)
{
	(void)ptr;
	(void)color;
	fprintf(stderr, "(%d, %d): %f\n", x, y, brightness);
}

void plotter_plot(void* ptr,
	int x, int y, float brightness, pixel_t color)
{
	assert(ptr != NULL);
	pg_t pg = *(pg_t*)ptr;
	if (0 <= x && x < (int)pg.w && 0 <= y && y < (int)pg.h)
	{
		#define CHANNEL(c_) \
			pg.pixel_grid[x + pg.w * y].c_ = \
				(1.0f - brightness) * pg.pixel_grid[x + pg.w * y].c_ + \
				brightness * color.c_
		CHANNEL(r);
		CHANNEL(g);
		CHANNEL(b);
		CHANNEL(a);
		#undef CHANNEL
	}
}

/* Pixel modification dynamic array. */
struct pm_da_t
{
	unsigned int len;
	unsigned int cap;
	pm_t* arr;
};
typedef struct pm_da_t pm_da_t;

void plotter_pm_da(void* ptr,
	int x, int y, float brightness, pixel_t color)
{
	assert(ptr != NULL);
	pm_da_t* pm_da = ptr;
	DA_LENGTHEN(++pm_da->len, pm_da->cap, pm_da->arr, pm_t);
	pm_da->arr[pm_da->len-1] = (pm_t){
		.x = x,
		.y = y,
		.r = color.r,
		.g = color.g,
		.b = color.b,
		.a = brightness * 255.0f,
	};
}

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

static float clamp(float x, float inf, float sup)
{
	if (x < inf)
	{
		return inf;
	}
	else if (x > sup)
	{
		return sup;
	}
	else
	{
		return x;
	}
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

#define W 1024
#define H 1024

void pg_init_white_disc(pg_t* pg, uint8_t alpha)
{
	assert(pg != NULL);
	*pg = (pg_t){
		.w = W,
		.h = H,
		.pixel_grid = malloc(W * H * sizeof(pixel_t)),
	};
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		if (dist(x, y, pg->w/2, pg->h/2) <= pg->w/2)
		{
			pg->pixel_grid[x + pg->w * y] = (pixel_t){
				.r = 255,
				.g = 255,
				.b = 255,
				.a = alpha,
			};
		}
	}
}

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

	#if 0

	pg_t target;
	pg_init_white_disc(&target, 0);
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
	pg_init_white_disc(&current, 255);

	pg_t trace;
	pg_init_white_disc(&trace, 255);

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
	output_pg_as_bitmap(current, output_file_path);
	output_pg_as_bitmap(trace, "trace.bmp");

	#endif

	#if 1

	pg_t pg;
	pg_init_white_disc(&pg, 255);

	line_xiaolin_wu(plotter_plot, &pg,
		(pixel_t){0, 255, 0, 255},
		300.0f, 300.0f, 600.0f, 761.0f);

	line_mid_point(plotter_plot, &pg,
		(pixel_t){0, 0, 255, 255},
		300.0f, 300.0f, 600.0f, 761.0f);

	output_pg_as_bitmap(pg, output_file_path);

	#endif

	return 0;
}
