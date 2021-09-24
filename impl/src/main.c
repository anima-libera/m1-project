
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "bmp.h"
#include "pg.h"
#include "line.h"

static inline float square(float x)
{
	return x * x;
}

static float dist(float xa, float ya, float xb, float yb)
{
	return sqrtf(square(xa - xb) + square(ya - yb));
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

	#define W 1024
	#define H 1024
	pg_t pg = {
		.w = W,
		.h = H,
		.pixel_grid = malloc(W * H * sizeof(pixel_t)),
	};
	for (unsigned int y = 0; y < pg.h; y++)
	for (unsigned int x = 0; x < pg.w; x++)
	{
		if (dist(x, y, pg.w/2, pg.h/2) <= pg.w/2)
		{
			pg.pixel_grid[x + pg.w * y] = (pixel_t){
				.r = 255,
				.g = 255,
				.b = 255,
				.a = 255,
			};
		}
	}

	line_naive(pg, (pixel_t){0, 0, 0, 255}, 20.0f, 20.0f, 100.0f, 1000.0f);

	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
