
#include "basics.h"

gs_op_t gs_op_combine(gs_op_t bottom, gs_op_t top)
{
	/* https://en.wikipedia.org/wiki/Alpha_compositing */
	gs_op_t result;
	result.op = top.op + bottom.op * (1.0f - top.op);
	result.gs = (top.gs * top.op + bottom.gs * bottom.op * (1.0f - top.op)) / result.op;
	return result;
}

float gs_op_combine_background(float background_gs, gs_op_t color)
{
	return color.gs * color.op + background_gs * (1.0f - color.op);
}

coords_grid_t coords_to_coords_grid(coords_t coords, unsigned int resolution)
{
	return (coords_grid_t){
		.x = coords.x * (float)(resolution - 1),
		.y = coords.y * (float)(resolution - 1),
	};
}

coords_t coords_grid_to_coords(coords_grid_t coords_grid, unsigned int resolution)
{
	/* Note: The formula used for reversing coords_to_coords_grid was chosen in a way
	 * that seem to maximize the number of resolutions that are good according to
	 * is_good_resolution, i.e. in which every coodinate can be converted from
	 * coords_grid_t to coords_t and then back to coords_grid_t without loss. */
	return (coords_t){
		.x = (float)coords_grid.x * (1.0f / (float)(resolution - 1)),
		.y = (float)coords_grid.y * (1.0f / (float)(resolution - 1)),
	};
}
