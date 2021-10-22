
#include "pg.h"
#include "utils.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

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

void pg_init_circles(pg_t* pg, pixel_t circles_color)
{
	pg_init_1024_white(pg);
	for (unsigned int y = 0; y < pg->h; y++)
	for (unsigned int x = 0; x < pg->w; x++)
	{
		if (dist(x, y, pg->w/2 - 200, pg->h/2) <= pg->w/7)
		{
			pg->pixel_grid[x + pg->w * y] = circles_color;
		}
		else if (dist(x, y, pg->w/2 - 150, pg->h/2 - 200) <= pg->w/8)
		{
			pg->pixel_grid[x + pg->w * y] = circles_color;
		}
		else if (dist(x, y, pg->w/2, pg->h/2 - 250) <= pg->w/18)
		{
			pg->pixel_grid[x + pg->w * y] = circles_color;
		}
	}
}

void pg_init_pic(pg_t* pg,
	const char* filepath_raw, const char* filepath_dim)
{
	printf("Reading dimensions from %s\n", filepath_dim);
	FILE* file_dim = fopen(filepath_dim, "r");
	fscanf(file_dim, "%u %u", &pg->w, &pg->h);
	fclose(file_dim);

	printf("Reading raw pixel data from %s\n", filepath_raw);
	unsigned int buffer_size = pg->w * pg->h * sizeof(pixel_t);
	pg->pixel_grid = malloc(buffer_size);
	FILE* file_raw = fopen(filepath_raw, "rb");
	fread(pg->pixel_grid, buffer_size, 1, file_raw);
	fclose(file_raw);
}

void pg_init_pic_name(pg_t* pg, const char* pic_name)
{
	unsigned int name_len = strlen(pic_name);
	char filepath_raw[11 + name_len + 4 + 1];
	char filepath_dim[11 + name_len + 4 + 1];
	sprintf(filepath_raw, "../rawpics/%s.raw", pic_name);
	sprintf(filepath_dim, "../rawpics/%s.dim", pic_name);
	pg_init_pic(pg, filepath_raw, filepath_dim);
}

void pg_grayscalize(pg_t pg)
{
	for (unsigned int i = 0; i < pg.w * pg.h; i++)
	{
		assert(i != INT_MAX);
		pixel_t p = pg.pixel_grid[i];
		unsigned int gray = (float)(p.r + p.g + p.b) / 3.0f;
		pg.pixel_grid[i] = (pixel_t){gray, gray, gray, 255};
	}
}

void pg_blackandwhiteize(pg_t pg)
{
	for (unsigned int i = 0; i < pg.w * pg.h; i++)
	{
		assert(i != INT_MAX);
		pixel_t p = pg.pixel_grid[i];
		unsigned int gray = (float)(p.r + p.g + p.b) / 3.0f;
		pg.pixel_grid[i] =
			gray > 0.5f ?
				(pixel_t){255, 255, 255, 255} :
				(pixel_t){0, 0, 0, 255};
	}
}

float pg_average_grayscale_in_disc(pg_t pg)
{
	float gray_sum = 0.0f;
	unsigned int pixel_count = 0;
	for (unsigned int y = 0; y < pg.h; y++)
	for (unsigned int x = 0; x < pg.w; x++)
	{
		if (dist(x, y, pg.w/2, pg.h/2) <= pg.w/2)
		{
			pixel_t p = pg.pixel_grid[x + pg.w * y];
			float gray = (float)(p.r + p.g + p.b) / 3.0f;
			gray_sum += gray / 255.0f;
			pixel_count++;
		}
	}
	return 1.0f - gray_sum / (float)pixel_count;
}
