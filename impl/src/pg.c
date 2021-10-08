
#include "pg.h"
#include "utils.h"
#include <assert.h>
#include <stdlib.h>

void pg_init_1024_white_disc(pg_t* pg, uint8_t alpha)
{
	assert(pg != NULL);
	#define SIDE 1024
	*pg = (pg_t){
		.w = SIDE,
		.h = SIDE,
		.pixel_grid = malloc(SIDE * SIDE * sizeof(pixel_t)),
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
				.a = alpha,
			};
		}
	}
}

void pg_init_1024_white(pg_t* pg, uint8_t alpha)
{
	assert(pg != NULL);
	#define SIDE 1024
	*pg = (pg_t){
		.w = SIDE,
		.h = SIDE,
		.pixel_grid = malloc(SIDE * SIDE * sizeof(pixel_t)),
	};
	#undef SIDE

	/* Draw white. */
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		pg->pixel_grid[x + pg->w * y] = (pixel_t){
			.r = 255,
			.g = 255,
			.b = 255,
			.a = alpha,
		};
	}
}
