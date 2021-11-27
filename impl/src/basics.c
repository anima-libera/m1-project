
#include "basics.h"

gs_op_t gs_op_combine(gs_op_t bottom, gs_op_t top)
{
	/* https://en.wikipedia.org/wiki/Alpha_compositing */
	gs_op_t result;
	result.op = top.op + bottom.op * (1.0f - top.op);
	result.gs = (top.gs * top.op + bottom.gs * bottom.op * (1.0f - top.op)) / result.op;
	return result;
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
	return (coords_t){
		.x = coords_grid.x / (float)(resolution - 1),
		.y = coords_grid.y / (float)(resolution - 1),
	};
}
