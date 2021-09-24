
/* Line drawing algorithms. */

#ifndef HEADER_LINE_
#define HEADER_LINE_

#include "pg.h"

void line_naive(pg_t pg, pixel_t color,
	float xa, float ya, float xb, float yb);

void line_naive_2(pg_t pg, pixel_t color,
	float xa, float ya, float xb, float yb);

#endif /* HEADER_LINE_ */
