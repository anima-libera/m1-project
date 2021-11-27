
#include "pinset.h"
#include "utils.h"
#include <stdlib.h>
#include <math.h>

pinset_t pinset_generate_circle(unsigned int pin_number)
{
	pinset_t pinset = {.pin_count = pin_number};
	pinset.pin_array = malloc(pin_number * sizeof(coords_t));
	for (unsigned int i = 0; i < pin_number; i++)
	{
		pinset.pin_array[i] = (coords_t){
			.x = cosf((float)i / (float)pin_number * TAU) * 0.5f + 0.5f,
			.y = sinf((float)i / (float)pin_number * TAU) * 0.5f + 0.5f,
		};
	}
	return pinset;
}
