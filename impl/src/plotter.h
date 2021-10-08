
/* Various plotter functions to be used with the line drawing functions. */

#ifndef HEADER_PLOTTER_
#define HEADER_PLOTTER_

#include "pg.h"

typedef void (*plotter_t)(void* ptr,
	int x, int y, float brightness, pixel_t color);

void plotter_print(void* ptr,
	int x, int y, float brightness, pixel_t color);

void plotter_plot(void* ptr,
	int x, int y, float brightness, pixel_t color);

/* Pixel modification dynamic array. */
struct pm_da_t
{
	unsigned int len;
	unsigned int cap;
	pm_t* arr;
};
typedef struct pm_da_t pm_da_t;

void plotter_pm_da(void* ptr,
	int x, int y, float brightness, pixel_t color);

#endif /* HEADER_PLOTTER_ */
