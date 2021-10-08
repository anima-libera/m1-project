
/* Pin related utilities. */

#ifndef HEADER_PINSET_
#define HEADER_PINSET_

#include "pg.h"

struct pinset_t
{
	unsigned int w, h;
	unsigned int pin_number;
};
typedef struct pinset_t pinset_t;

/* Fills out_x and out_y with the coordinates of the pin of the given index
 * in the given pinset. Note that even if the given index is out of bounds,
 * teh pin coordinates will still be filled out as the index may warp.
 * Returns 0 if the given index is out of bounds, 1 otherwise. */
int pinset_get_pin_pos(pinset_t pinset, int pin_index,
	float* out_x, float* out_y);

#endif /* HEADER_PINSET_ */
