
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "utils.h"

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

	#define N 400
	for (unsigned int i = 0; i < N; i++)
	{
		line_xiaolin_wu(pg, (pixel_t){100, 0, 0, 255},
			(float)(pg.w/2) + (float)(pg.w/2) * 0.3f * cosf((0.1f + (float)i / (float)N) * (float)TAU),
			(float)(pg.h/2) + (float)(pg.h/2) * 0.3f * sinf((0.1f + (float)i / (float)N) * (float)TAU),
			(float)(pg.w/2) + (float)(pg.w/2) * cosf((float)i / (float)N * (float)TAU),
			(float)(pg.h/2) + (float)(pg.h/2) * sinf((float)i / (float)N * (float)TAU));
	}
	
	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
