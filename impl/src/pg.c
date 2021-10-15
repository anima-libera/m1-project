
#include "pg.h"
#include "utils.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>

void pg_init_1024_white_disc(pg_t* pg)
{
	assert(pg != NULL);
	#define SIDE 1024
	*pg = (pg_t){
		.w = SIDE,
		.h = SIDE,
		.pixel_grid = calloc(SIDE * SIDE, sizeof(pixel_t)),
	};
	#undef SIDE

	/* Draw white disc. */
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		if (dist(x, y, pg->w/2, pg->h/2) <= pg->w/2)
		{
			pg->pixel_grid[x + pg->w * y] = (pixel_t){
				.r = 255,
				.g = 255,
				.b = 255,
				.a = 255,
			};
		}
	}
}

void pg_init_1024_white(pg_t* pg)
{
	assert(pg != NULL);
	#define SIDE 1024
	*pg = (pg_t){
		.w = SIDE,
		.h = SIDE,
		.pixel_grid = malloc(SIDE * SIDE * sizeof(pixel_t)),
	};
	#undef SIDE

	/* Fill in white. */
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		pg->pixel_grid[x + pg->w * y] = (pixel_t){
			.r = 255,
			.g = 255,
			.b = 255,
			.a = 255,
		};
	}
}

void pg_init_white(pg_t* pg, unsigned int w, unsigned int h)
{
	assert(pg != NULL);
	*pg = (pg_t){
		.w = w,
		.h = h,
		.pixel_grid = malloc(w * h * sizeof(pixel_t)),
	};

	/* Fill in white. */
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		pg->pixel_grid[x + pg->w * y] = (pixel_t){
			.r = 255,
			.g = 255,
			.b = 255,
			.a = 255,
		};
	}
}

void pg_init_copy(pg_t* pg_dst, pg_t pg_src)
{
	assert(pg_dst != NULL);
	*pg_dst = (pg_t){
		.w = pg_src.w,
		.h = pg_src.h,
		.pixel_grid = malloc(pg_src.w * pg_src.h * sizeof(pixel_t)),
	};

	memcpy(pg_dst->pixel_grid, pg_src.pixel_grid,
		pg_src.w * pg_src.h * sizeof(pixel_t));
}
