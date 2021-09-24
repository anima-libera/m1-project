
#include <math.h>
#include <assert.h>
#include "pg.h"

void line_naive(pg_t pg, pixel_t color,
	float xa, float ya, float xb, float yb)
{
	const float dx = xb - xa;
	const float dy = yb - ya;
	const float square_len = dx*dx + dy*dy;
	const float len = sqrtf(square_len);
	assert(len != 0.0f);

	const float ndx = dx / len;
	const float ndy = dy / len;
	const float step = 0.5f; /* Smallest radius of pixel (square of side 1). */
	
	float x = xa;
	float y = ya;
	int ix = roundf(x);
	int iy = roundf(y);
	float square_dist = (x-xb)*(x-xb) + (y-yb)*(y-yb);
	int ix_new;
	int iy_new;
	float square_dist_new;
	pg.pixel_grid[ix + pg.w * iy] = color;
	while (1)
	{
		while ((ix_new = roundf(x)), (iy_new = roundf(y)),
			ix_new == ix && iy_new == iy)
		{
			x += ndx * step;
			y += ndy * step;
		}
		ix = ix_new;
		iy = iy_new;
		square_dist_new = (x-xb)*(x-xb) + (y-yb)*(y-yb);
		if (square_dist_new >= square_dist)
		{
			break;
		}
		square_dist = square_dist_new;
		pg.pixel_grid[ix + pg.w * iy] = color;
	}
}
