
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

void pg_init_circles(pg_t* pg, pixel_t circles_color);

void pg_init_pic(pg_t* pg,
	const char* filepath_raw, const char* filepath_dim);

void pg_init_pic_name(pg_t* pg, const char* pic_name);

void pg_grayscalize(pg_t pg);

void pg_blackandwhiteize(pg_t pg);

float pg_average_grayscale_in_disc(pg_t pg);

#endif /* HEADER_PG_ */
