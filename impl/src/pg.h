
/* Pixel grid data structure and related utilities. */

#ifndef HEADER_PG_
#define HEADER_PG_

#include <stdint.h>

struct pixel_t
{
	uint8_t r, g, b, a;
};
typedef struct pixel_t pixel_t;

/* Pixel grid pointer.
 * Can be seen as a "fat pointer". */
struct pg_t
{
	unsigned int w, h;
	pixel_t* pixel_grid;
};
typedef struct pg_t pg_t;

/* Pixel modification.
 * When one pixel if modified by a line that might be drawn,
 * this struct sums up what pixel is being modified and how.
 * The alpha channel is used by algorithms that plots with
 * a non-0-or-1 brightness. */
struct pm_t
{
	unsigned int x, y;
	uint8_t r, g, b, a;
};
typedef struct pm_t pm_t;

#endif /* HEADER_PG_ */
