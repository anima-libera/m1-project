
/* Line drawing algorithms. */

#ifndef HEADER_LINE_
#define HEADER_LINE_

#include "pg.h"
#include "plotter.h"

struct line_t
{
	float xa, xb, ya, yb;
	pixel_t color;
};
typedef struct line_t line_t;

void line_xiaolin_wu(plotter_t plot, void* ptr, line_t line);

void line_mid_point(plotter_t plot, void* ptr, line_t line);

#endif /* HEADER_LINE_ */
