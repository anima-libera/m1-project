
#include "line.h"
#include "pg.h"
#include <math.h>
#include <stdlib.h> /* abs */
#include <assert.h>
#include <stdio.h>

/* Maybe remove ? It was a test anyway... */
#if 0
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
	if (0 <= ix && ix < (int)pg.w && 0 <= iy && iy < (int)pg.h)
	{
		pg.pixel_grid[ix + pg.w * iy] = color;
	}
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
		if (0 <= ix && ix < (int)pg.w && 0 <= iy && iy < (int)pg.h)
		{
			pg.pixel_grid[ix + pg.w * iy] = color;
		}
	}
}

void line_naive_2(pg_t pg, pixel_t color,
	float xa, float ya, float xb, float yb)
{
	const float dx = xb - xa;
	const float dy = yb - ya;
	const float square_len = dx*dx + dy*dy;
	const float len = sqrtf(square_len);
	assert(len != 0.0f);

	const float ndx = dx / len;
	const float ndy = dy / len;
	const float step = 0.01f;

	#define FACTOR 0.013f
	
	float x = xa;
	float y = ya;
	int ix = roundf(x);
	int iy = roundf(y);
	float square_dist = (x-xb)*(x-xb) + (y-yb)*(y-yb);
	int ix_new;
	int iy_new;
	float square_dist_new;
	while (1)
	{
		while ((ix_new = roundf(x)), (iy_new = roundf(y)),
			ix_new == ix && iy_new == iy)
		{
			if (0 <= ix && ix < (int)pg.w && 0 <= iy && iy < (int)pg.h)
			{
				#define CHANNEL(c_) \
					pg.pixel_grid[ix + pg.w * iy].c_ = \
						(1.0f - FACTOR) * pg.pixel_grid[ix + pg.w * iy].c_ + \
						FACTOR * color.c_
				CHANNEL(r);
				CHANNEL(g);
				CHANNEL(b);
				CHANNEL(a);
				#undef CHANNEL
			}
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
	}
}
#endif

static inline float frac(float x)
{
	return x - floorf(x);
}

/* TODO: Understand and comment. */
void line_xiaolin_wu(plotter_t plot, void* ptr, pixel_t color,
	float xa, float ya, float xb, float yb)
{
	const int is_steep = fabsf(yb - ya) > fabsf(xb - xa);

	#define SWAP(a_, b_) do { float tmp = a_; a_ = b_; b_ = tmp; } while (0)
	if (is_steep)
	{
		SWAP(xa, ya);
		SWAP(xb, yb);
	}
	if (xa > xb)
	{
		SWAP(xa, xb);
		SWAP(ya, yb);
	}
	#undef SWAP

	const float dx = xb - xa;
	const float dy = yb - ya;
	const float gradient = dx == 0.0f ? 1.0f : dy / dx;

	#define PLOT(x_, y_, brightness_) \
		do \
		{ \
			if (is_steep) \
			{ \
				plot(ptr, (y_), (x_), (brightness_), color); \
			} \
			else \
			{ \
				plot(ptr, (x_), (y_), (brightness_), color); \
			} \
		} while (0)

	/* End point A. */
	float intery;
	int xpa;
	{
		const float xend = roundf(xa);
		const float yend = ya + gradient * (xend - xa);
		const float xgap = 1.0f - frac(xa + 0.5f);
		xpa = xend;
		const int ypa = floorf(yend);
		PLOT(xpa, ypa, (1.0f - frac(yend)) * xgap);
		PLOT(xpa, ypa + 1, frac(yend) * xgap);
		intery = yend + gradient;
	}

	/* End point B. */
	int xpb;
	{
		const float xend = roundf(xb);
		const float yend = yb + gradient * (xend - xb);
		const float xgap = frac(xb + 0.5f);
		xpb = xend;
		const int ypb = floorf(yend);
		PLOT(xpb, ypb, (1.0f - frac(yend)) * xgap);
		PLOT(xpb, ypb + 1, frac(yend) * xgap);
	}

	/* The actual line. */
	for (int x = roundf(xpa + 1.0f); x <= roundf(xpb - 1.0f); x++)
	{
		PLOT((float)x, floorf(intery), 1.0f - frac(intery));
		PLOT((float)x, floorf(intery) + 1.0f, frac(intery));
		intery += gradient;
	}
}

/* TODO: Understand and comment. */
void line_mid_point(plotter_t plot, void* ptr, pixel_t color,
	int xa, int ya, int xb, int yb)
{
	int dx = abs(xb - xa);
	int sx = xa < xb ? 1 : -1;
	int dy = -abs(yb - ya);
	int sy = ya < yb ? 1 : -1;
	int err = dx + dy;
	while (1)
	{
		plot(ptr, xa, ya, 1.0f, color);
		if (xa == xb && ya == yb)
		{
			break;
		}
		int e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			xa += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			ya += sy;
		}
	}
}
