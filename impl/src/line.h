
#ifndef HEADER_LINE_
#define HEADER_LINE_

#include "basics.h"

struct line_pins_t
{
	unsigned int pin_index_a, pin_index_b;
	gs_op_t color;
};
typedef struct line_pins_t line_pins_t;

struct line_coords_t
{
	coords_t coords_a, coords_b;
	gs_op_t color;
};
typedef struct line_coords_t line_coords_t;

float line_coords_length(line_coords_t line);

struct pixel_line_t
{
	coords_t coords;
	gs_op_t color;
};
typedef struct pixel_line_t pixel_line_t;

typedef void (*plotter_t)(void* plot_data, pixel_line_t pixel);

/* Plot the given line using Xiaolin Wu's line drawing algorithm that
 * produces anti-aliased lines.
 * Pixels are plotted using the given plot function callback called
 * on the given plot data.
 * It never plots more than once on any given coords. */
void line_plot_pixels_xiaolin_wu(plotter_t plotter, void* plotter_data, line_coords_t line,
	unsigned int resolution);

/* Plot the given line using the mid point line drawing algorithm that
 * produces lines quickly and with only whole pixels
 * (no anti-aliasing like Xiaolin Wu's algorithm).
 * Pixels are plotted using the given plot function callback called
 * on the given plot data.
 * It never plots more than once on any given coords. */
void line_plot_pixels_mid_point(plotter_t plotter, void* plotter_data, line_coords_t line,
	unsigned int resolution);

typedef void (*line_plot_algorithm_t)(plotter_t plotter, void* plotter_data, line_coords_t line,
	unsigned int resolution);

/* Line represented as the array of pixels it modifies when drawn
 * on a pixel grid of the associated resolution. */
struct line_pixels_t
{
	unsigned int pixel_count;
	pixel_line_t* pixel_array;
	unsigned int resolution;
};
typedef struct line_pixels_t line_pixels_t;

/* Returns the allocated line pixels corresponding to the given line in the given resolution. */
line_pixels_t line_coords_to_line_pixels(line_coords_t line, unsigned int resolution,
	line_plot_algorithm_t line_algorithm);

/* Returns the allocated line pixels corresponding to the given line
 * in the given smaller resolution,
 * basically merging small HD pixels together into bigger SD pixels. */
line_pixels_t line_pixels_downscale_hd_to_sd(line_pixels_t line_hd, unsigned int resolution_sd);

/* Deallocates the allocated pixels. */
void line_pixels_cleanup(line_pixels_t line);

#endif /* HEADER_LINE_ */
