
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

pinset_t pinset_generate_square(unsigned int pin_number)
{
	/* TODO: Fix the fact that it seems like there is one space between some pins on each side
	 * that make it look like there is a pin missing on each side. */
	pinset_t pinset = {.pin_count = pin_number};
	pinset.pin_array = malloc(pin_number * sizeof(coords_t));
	for (unsigned int i = 0; i < pin_number; i++)
	{
		pinset.pin_array[i] =
			i * 4 / pin_number == 0 ?
				(coords_t){
					.x = (float)(i % (pin_number / 4)) * 4.0f / (float)pin_number,
					.y = 0.0f,
				} :
			i * 4 / pin_number == 1 ?
				(coords_t){
					.x = 1.0f,
					.y = (float)(i % (pin_number / 4)) * 4.0f / (float)pin_number,
				} :
			i * 4 / pin_number == 2 ?
				(coords_t){
					.x = 1.0f - (float)(i % (pin_number / 4)) * 4.0f / (float)pin_number,
					.y = 1.0f,
				} :
			i * 4 / pin_number == 3 ?
				(coords_t){
					.x = 0.0f,
					.y = 1.0f - (float)(i % (pin_number / 4)) * 4.0f / (float)pin_number,
				} :
			(assert(0), (coords_t){0});
	}
	return pinset;
}
