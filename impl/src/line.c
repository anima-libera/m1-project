
#include "line.h"
#include "utils.h"
#include <math.h>
#include <assert.h>
#include <stdlib.h> /* fabsf */

float line_coords_length(line_coords_t line)
{
	return dist(line.coords_a.x, line.coords_a.y, line.coords_b.x, line.coords_b.y);
}

static gs_op_t gs_op_times_op(gs_op_t color, float op)
{
	return (gs_op_t){.gs = color.gs, .op = color.op * op};
}

static inline float frac(float x)
{
	return x - floorf(x);
}

void line_plot_pixels_xiaolin_wu(plotter_t plotter, void* plotter_data, line_coords_t line,
	unsigned int resolution)
{
	coords_grid_t coords_grid_a = coords_to_coords_grid(line.coords_a, resolution);
	coords_grid_t coords_grid_b = coords_to_coords_grid(line.coords_b, resolution);
	int xa = coords_grid_a.x;
	int ya = coords_grid_a.y;
	int xb = coords_grid_b.x;
	int yb = coords_grid_b.y;

	const int is_steep = fabsf(yb - ya) > fabsf(xb - xa);

	if (is_steep)
	{
		SWAP(int, xa, ya);
		SWAP(int, xb, yb);
	}
	if (xa > xb)
	{
		SWAP(int, xa, xb);
		SWAP(int, ya, yb);
	}

	const float dx = xb - xa;
	const float dy = yb - ya;
	const float gradient = dx == 0.0f ? 1.0f : dy / dx;

	#define PLOT(x_, y_, brightness_) \
		do \
		{ \
			int xx = (x_), yy = (y_); \
			if (is_steep) \
			{ \
				SWAP(int, xx, yy); \
			} \
			plotter(plotter_data, (pixel_line_t){ \
				.coords = coords_grid_to_coords((coords_grid_t){xx, yy}, resolution), \
				.color = gs_op_times_op(line.color, (brightness_)), \
			}); \
		} while (0)

	/* End point A. */
	float intery;
	int xpa;
	{
		const float xend = roundf(xa);
		const float yend = ya + gradient * (xend - xa);
		const float xgap = 1.0f - frac(xa + 0.5f);
		xpa = xend;
		const int ypa = floorf(yend);
		PLOT(xpa, ypa, (1.0f - frac(yend)) * xgap);
		PLOT(xpa, ypa + 1, frac(yend) * xgap);
		intery = yend + gradient;
	}

	/* End point B. */
	int xpb;
	{
		const float xend = roundf(xb);
		const float yend = yb + gradient * (xend - xb);
		const float xgap = frac(xb + 0.5f);
		xpb = xend;
		const int ypb = floorf(yend);
		PLOT(xpb, ypb, (1.0f - frac(yend)) * xgap);
		PLOT(xpb, ypb + 1, frac(yend) * xgap);
	}

	/* The actual line. */
	for (int x = roundf(xpa + 1.0f); x <= roundf(xpb - 1.0f); x++)
	{
		PLOT((float)x, floorf(intery), 1.0f - frac(intery));
		PLOT((float)x, floorf(intery) + 1.0f, frac(intery));
		intery += gradient;
	}
}

void line_plot_pixels_mid_point(plotter_t plotter, void* plotter_data, line_coords_t line,
	unsigned int resolution)
{
	const coords_grid_t coords_grid_a = coords_to_coords_grid(line.coords_a, resolution);
	const coords_grid_t coords_grid_b = coords_to_coords_grid(line.coords_b, resolution);
	int xa = coords_grid_a.x;
	int ya = coords_grid_a.y;
	const int xb = coords_grid_b.x;
	const int yb = coords_grid_b.y;

	const int dx = abs(xb - xa);
	const int sx = xa < xb ? 1 : -1;
	const int dy = -abs(yb - ya);
	const int sy = ya < yb ? 1 : -1;
	int err = dx + dy;
	while (1)
	{
		plotter(plotter_data, (pixel_line_t){
			.coords = coords_grid_to_coords((coords_grid_t){xa, ya}, resolution), \
			.color = line.color,
		});
		if (xa == xb && ya == yb)
		{
			break;
		}
		int e2 = 2 * err;
		if (e2 >= dy)
		{
			err += dy;
			xa += sx;
		}
		if (e2 <= dx)
		{
			err += dx;
			ya += sy;
		}
	}
}

struct line_pixels_da_t
{
	line_pixels_t line_pixels;
	unsigned int capacity;
};
typedef struct line_pixels_da_t line_pixels_da_t;

static void line_pixels_da_plotter(void* plot_data, pixel_line_t pixel)
{
	line_pixels_da_t* da = plot_data;
	DA_LENGTHEN(da->line_pixels.pixel_count += 1, da->capacity,
		da->line_pixels.pixel_array, pixel_line_t);
	da->line_pixels.pixel_array[da->line_pixels.pixel_count - 1] = pixel;
}

line_pixels_t line_coords_to_line_pixels(line_coords_t line, unsigned int resolution,
	line_plot_algorithm_t line_algorithm)
{
	/* Preallocating a bit more than an estimation of the number of pixels of the line,
	 * which is about resolution * length. */
	const unsigned int initial_capacity =
		line_coords_length(line) * resolution + resolution / 40;
	line_pixels_da_t da = {
		.line_pixels = {
			.pixel_array = malloc(initial_capacity * sizeof(pixel_line_t)),
			.resolution = resolution,
		},
		.capacity = initial_capacity,
	};
	line_algorithm(line_pixels_da_plotter, &da, line, resolution);
	da.line_pixels.pixel_array = realloc(da.line_pixels.pixel_array,
		da.line_pixels.pixel_count * sizeof(pixel_line_t));
	return da.line_pixels;
}

line_pixels_t line_pixels_downscale_hd_to_sd(line_pixels_t line_hd, unsigned int resolution_sd)
{
	/* Preallocating a bit more than an estimation of the number of pixels of the line,
	 * which is about resolution * length. */
	const unsigned int initial_capacity_sd =
		line_hd.pixel_count / (line_hd.resolution / resolution_sd) + resolution_sd / 40;
	line_pixels_da_t da_sd = {
		.line_pixels = {
			.pixel_array = malloc(initial_capacity_sd * sizeof(pixel_line_t)),
			.resolution = resolution_sd,
		},
		.capacity = initial_capacity_sd,
	};

	const float pixel_dilution_opacity = 1.0f / (float)(
		(line_hd.resolution / resolution_sd) * (line_hd.resolution / resolution_sd));

	/* This rectangle describes an area that contains all coords_sd already in the da_sd
	 * so that we can avoid iterating over it when searching for coords outside of the area. */
	float min_x_sd = 1.0f, max_x_sd = 0.0f, min_y_sd = 1.0f, max_y_sd = 0.0f;

	for (unsigned int i = 0; i < line_hd.pixel_count; i++)
	{
		const pixel_line_t pixel_hd = line_hd.pixel_array[i];

		/* The coords of the pixel in SD that "contains" the current HD pixel. */
		const coords_t coords_sd = coords_grid_to_coords(
			coords_to_coords_grid(pixel_hd.coords, resolution_sd), resolution_sd);

		/* If there is already an SD pixel in the da_sd with coords_sd as its coords, then we
		 * have to find it to modify it instead of adding a redundant SD pixel. Checking for
		 * the previously mentionned rectangle and iterating from the end of the da_sd should
		 * make the search fast.
		 * If there is no SD pixel with coords_sd as its coords, then we add a fully transparent
		 * SD pixel with these coords, and we expand the rectangle. */
		pixel_line_t* pixel_sd = NULL;
		if (min_x_sd <= coords_sd.x && coords_sd.x <= max_x_sd &&
			min_y_sd <= coords_sd.y && coords_sd.y <= max_y_sd)
		{
			for (int j = da_sd.line_pixels.pixel_count - 1; j > 0; j--)
			{
				const coords_t pixel_j_coords = da_sd.line_pixels.pixel_array[j].coords;
				if (pixel_j_coords.x == coords_sd.x && pixel_j_coords.y == coords_sd.y)
				{
					pixel_sd = &da_sd.line_pixels.pixel_array[j];
					break;
				}
			}
		}
		if (pixel_sd == NULL)
		{
			DA_LENGTHEN(da_sd.line_pixels.pixel_count += 1, da_sd.capacity,
				da_sd.line_pixels.pixel_array, pixel_line_t);
			pixel_sd = &da_sd.line_pixels.pixel_array[da_sd.line_pixels.pixel_count - 1];
			*pixel_sd = (pixel_line_t){.coords = coords_sd, .color = {.op = 0.0f}};
			
			if (pixel_sd->coords.x < min_x_sd)
			{
				min_x_sd = pixel_sd->coords.x;
			}
			else if (pixel_sd->coords.x > max_x_sd)
			{
				max_x_sd = pixel_sd->coords.x;
			}
			if (pixel_sd->coords.y < min_y_sd)
			{
				min_y_sd = pixel_sd->coords.y;
			}
			else if (pixel_sd->coords.y > max_y_sd)
			{
				max_y_sd = pixel_sd->coords.y;
			}
		}

		pixel_sd->color = gs_op_combine(pixel_sd->color,
			gs_op_times_op(pixel_hd.color, pixel_dilution_opacity));
	}

	da_sd.line_pixels.pixel_array = realloc(da_sd.line_pixels.pixel_array,
		da_sd.line_pixels.pixel_count * sizeof(pixel_line_t));
	return da_sd.line_pixels;
}

void line_pixels_cleanup(line_pixels_t line)
{
	free(line.pixel_array);
}
