
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
pinset_t pinset_generate_square(unsigned int pin_number);
pinset_t pinset_generate_random(unsigned int pin_number);
pinset_t pinset_generate_x(unsigned int pin_number);
pinset_t pinset_generate_circle_center(unsigned int pin_number);

#endif /* HEADER_PINSET_ */
