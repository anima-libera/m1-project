
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "plotter.h"
#include "utils.h"
#include "pin.h"
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

	int pin_index = 0;
	float xa, ya, xb, yb;
	pg_get_pin_pos(pg, pin_index++, &xa, &ya);
	while (pg_get_pin_pos(pg, pin_index++, &xb, &yb))
	{
		line_xiaolin_wu(plotter_plot, &pg,
			(pixel_t){255, 0, 0, 255},
			xa, ya, xb, yb);
	}

	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
