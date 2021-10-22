
#include "plotter.h"
#include "utils.h"
#include "pg.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

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

void plotter_pm_da(void* ptr,
	int x, int y, float brightness, pixel_t color)
{
	assert(ptr != NULL);
	pm_da_t* pm_da = ptr;
	/* TODO: Scan for an (x, y) pm first. */
	DA_LENGTHEN(++pm_da->len, pm_da->cap, pm_da->arr, pm_t);
	pm_da->arr[pm_da->len-1] = (pm_t){
		.x = x,
		.y = y,
		.r = color.r,
		.g = color.g,
		.b = color.b,
		.a = brightness == 1.0f ? 255 : brightness * 255.0f,
	};
}
