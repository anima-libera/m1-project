
#include "bmp.h"
#include "pg.h"
#include "line.h"
#include "plotter.h"
#include "utils.h"
#include "pinset.h"
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
	pinset_t pinset = {.w = pg.w, .h = pg.h, .pin_number = 32};

	int pin_index_a = 0;
	float xa, ya;
	while (pinset_get_pin_pos(pinset, pin_index_a++, &xa, &ya))
	{
		int pin_index_b = 0;
		float xb, yb;
		while (pinset_get_pin_pos(pinset, pin_index_b++, &xb, &yb))
		{
			line_xiaolin_wu(plotter_plot, &pg,
				(pixel_t){255, 0, 0, 255},
				xa, ya, xb, yb);
		}
	}

	output_pg_as_bitmap(pg, output_file_path);

	return 0;
}
