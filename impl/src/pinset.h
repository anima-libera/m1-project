
#ifndef HEADER_PINSET_
#define HEADER_PINSET_

#include "basics.h"

struct pinset_t
{
	unsigned int pin_count;
	coords_t* pin_array;
};
typedef struct pinset_t pinset_t;

pinset_t pinset_generate_circle(unsigned int pin_number);

#endif /* HEADER_PINSET_ */
