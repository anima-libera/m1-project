
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

	pg_t pg;
	pg_init_1024_white_disc(&pg, 255);

	line_xiaolin_wu(plotter_plot, &pg,
		(pixel_t){0, 255, 0, 255},
		300.0f, 300.0f, 600.0f, 761.0f);

	line_mid_point(plotter_plot, &pg,
		(pixel_t){0, 0, 255, 255},
		300.0f, 300.0f, 600.0f, 761.0f);

	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
