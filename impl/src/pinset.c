
#include "pinset.h"
#include "pg.h"
#include "utils.h"
#include <assert.h>
#include <stddef.h>
#include <math.h>

int pinset_get_pin_pos(pinset_t pinset, int pin_index,
	float* out_x, float* out_y)
{
	assert(out_x != NULL);
	assert(out_y != NULL);

	float angle = (float)pin_index / (float)pinset.pin_number * TAU;
	float half_w = (float)pinset.w / 2.0f;
	float half_h = (float)pinset.h / 2.0f;

	*out_x = half_w + half_w * cosf(angle);
	*out_y = half_h + half_h * sinf(angle);

	return 0 <= pin_index && pin_index < (int)pinset.pin_number;
}
