
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

static void plot(pg_t pg, pixel_t color, int ix, int iy, float brightness)
{
	if (0 <= ix && ix < (int)pg.w && 0 <= iy && iy < (int)pg.h)
	{
		#define CHANNEL(c_) \
			pg.pixel_grid[ix + pg.w * iy].c_ = \
				(1.0f - brightness) * pg.pixel_grid[ix + pg.w * iy].c_ + \
				brightness * color.c_
		CHANNEL(r);
		CHANNEL(g);
		CHANNEL(b);
		CHANNEL(a);
		#undef CHANNEL
	}
}

static inline float frac(float x)
{
	return x - floorf(x);
}

void line_xiaolin_wu(pg_t pg, pixel_t color,
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
				plot(pg, color, (y_), (x_), brightness_); \
			} \
			else \
			{ \
				plot(pg, color, (x_), (y_), brightness_); \
			} \
		} while (0)

	float intery;
	float xpxl1;
	{
		const float xend = roundf(xa);
		const float yend = ya + gradient * (xend - xa);
		const float xgap = 1.0f - frac(xa + 0.5f);
		xpxl1 = xend;
		const float ypxl1 = floorf(yend);
		PLOT(xpxl1, ypxl1, (1.0f - frac(yend)) * xgap);
		PLOT(xpxl1, ypxl1 + 1, frac(yend) * xgap);
		intery = yend + gradient;
	}

	float xpxl2;
	{
		const float xend = roundf(xb);
		const float yend = yb + gradient * (xend - xb);
		const float xgap = frac(xb + 0.5f);
		xpxl2 = xend;
		const float ypxl2 = floorf(yend);
		PLOT(xpxl2, ypxl2, (1.0f - frac(yend)) * xgap);
		PLOT(xpxl2, ypxl2 + 1, frac(yend) * xgap);
	}

	for (int x = xpxl1 + 1.0f; x < xpxl2 - 1.0f; x++)
	{
		PLOT((float)x, floorf(intery), 1.0f - frac(intery));
		PLOT((float)x, floorf(intery) + 1.0f, frac(intery));
		intery += gradient;
	}
}

#if 0
function plot(x, y, c) is
	plot the pixel at (x, y) with brightness c (where 0 ≤ c ≤ 1)

function drawLine(x0,y0,x1,y1) is
	boolean steep := abs(y1 - y0) > abs(x1 - x0)
	
	if steep then
		swap(x0, y0)
		swap(x1, y1)
	end if
	if x0 > x1 then
		swap(x0, x1)
		swap(y0, y1)
	end if
	
	dx := x1 - x0
	dy := y1 - y0
	gradient := dy / dx
	if dx == 0.0 then
		gradient := 1.0
	end if

	// handle first endpoint
	xend := round(x0)
	yend := y0 + gradient * (xend - x0)
	xgap := rfpart(x0 + 0.5)
	xpxl1 := xend // this will be used in the main loop
	ypxl1 := ipart(yend)
	if steep then
		plot(ypxl1,   xpxl1, rfpart(yend) * xgap)
		plot(ypxl1+1, xpxl1,  fpart(yend) * xgap)
	else
		plot(xpxl1, ypxl1  , rfpart(yend) * xgap)
		plot(xpxl1, ypxl1+1,  fpart(yend) * xgap)
	end if
	intery := yend + gradient // first y-intersection for the main loop
	
	// handle second endpoint
	xend := round(x1)
	yend := y1 + gradient * (xend - x1)
	xgap := fpart(x1 + 0.5)
	xpxl2 := xend //this will be used in the main loop
	ypxl2 := ipart(yend)
	if steep then
		plot(ypxl2  , xpxl2, rfpart(yend) * xgap)
		plot(ypxl2+1, xpxl2,  fpart(yend) * xgap)
	else
		plot(xpxl2, ypxl2,  rfpart(yend) * xgap)
		plot(xpxl2, ypxl2+1, fpart(yend) * xgap)
	end if
	
	// main loop
	if steep then
		for x from xpxl1 + 1 to xpxl2 - 1 do
			begin
				plot(ipart(intery)  , x, rfpart(intery))
				plot(ipart(intery)+1, x,  fpart(intery))
				intery := intery + gradient
			end
	else
		for x from xpxl1 + 1 to xpxl2 - 1 do
			begin
				plot(x, ipart(intery),  rfpart(intery))
				plot(x, ipart(intery)+1, fpart(intery))
				intery := intery + gradient
			end
	end if
end function
#endif
