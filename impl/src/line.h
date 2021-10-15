
/* Line drawing algorithms. */

#ifndef HEADER_LINE_
#define HEADER_LINE_

#include "pg.h"
#include "plotter.h"

void line_xiaolin_wu(plotter_t plot, void* ptr, pixel_t color,
	float xa, float ya, float xb, float yb);

void line_mid_point(plotter_t plot, void* ptr, pixel_t color,
	int xa, int ya, int xb, int yb);

#endif /* HEADER_LINE_ */
