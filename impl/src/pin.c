
#include "pg.h"
#include "utils.h"
#include <assert.h>
#include <stddef.h>
#include <math.h>

#define PIN_NUMBER 128

int pg_get_pin_pos(pg_t pg, int pin_index,
	float* out_x, float* out_y)
{
	assert(out_x != NULL);
	assert(out_y != NULL);

	float angle = (float)pin_index / (float)PIN_NUMBER * TAU;
	float half_w = (float)pg.w / 2.0f;
	float half_h = (float)pg.h / 2.0f;

	*out_x = half_w + half_w * cosf(angle);
	*out_y = half_h + half_h * sinf(angle);

	return 0 <= pin_index && pin_index < PIN_NUMBER;
}
