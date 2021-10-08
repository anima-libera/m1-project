
/* Pin related utilities. */

#ifndef HEADER_PIN_
#define HEADER_PIN_

#include "pg.h"

/* Fills out_x and out_y with the coordinates of the pin of the given index
 * in the given pg. Note that even if the given index is out of bounds,
 * teh pin coordinates will still be filled out as the index may warp.
 * Returns 0 if the given index is out of bounds, 1 otherwise. */
int pg_get_pin_pos(pg_t pg, int pin_index,
	float* out_x, float* out_y);

#endif /* HEADER_PIN_ */
