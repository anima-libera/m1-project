
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include "bmp.h"
#include "pg.h"

static inline float square(float x)
{
	return x * x;
}

static float dist(float xa, float ya, float xb, float yb)
{
	return sqrtf(square(xa - xb) + square(ya - yb));
}

int main(int argc, char** argv)
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
				.r = x * 255 / W,
				.g = y * 255 / H,
				.b = (x - y) - 2*(x + y),
				.a = 255,
			};
		}
	}

	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
