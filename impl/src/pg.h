
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

/* Initializes the given pg, allocates the pixel grid,
 * and draws a white disc (the biggets that fits)
 * on a transparent black background. */
void pg_init_1024_white_disc(pg_t* pg);

/* Initializes the given pg, allocates the pixel grid,
 * and fills it in white. */
void pg_init_1024_white(pg_t* pg);

/* Initializes the given pg, allocates the pixel grid,
 * and fills it in white. */
void pg_init_white(pg_t* pg, unsigned int w, unsigned int h);

/* Initializes the given dst pg, allocates the pixel grid,
 * and fills it with the content of the src pg, making a copy. */
void pg_init_copy(pg_t* pg_dst, pg_t pg_src);

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
