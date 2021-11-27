
#ifndef HEADER_BASICS_
#define HEADER_BASICS_

/* Color in grayscale with an opacity component. */
struct gs_op_t
{
	float gs; /* Grayscale between 0.0f and 1.0f, 1.0f meaning black. */
	float op; /* Opacity between 0.0f and 1.0f, 1.0f meaning fully opaque. */
};
typedef struct gs_op_t gs_op_t;

/* Returns the color obtained when painting the given top color on the given bottom color. */
gs_op_t gs_op_combine(gs_op_t bottom, gs_op_t top);

/* Coordinates of a point on all the possible square canvases, regardless of their resolutions.
 * Should be used every time it is possible. */
struct coords_t
{
	float x, y; /* Each between 0.0f and 1.0f. */
};
typedef struct coords_t coords_t;

/* Coordinates of a specific pixel in a grid of pixels,
 * only makes sense when associated with a pixel grid resolution. */
struct coords_grid_t
{
	int x, y; /* Each between 0 and resolution-1. */
};
typedef struct coords_grid_t coords_grid_t;

/* Properly maps coords to coords_grid.
 * Should be used instead of the multiplication operator. */
coords_grid_t coords_to_coords_grid(coords_t coords, unsigned int resolution);

/* Properly maps coords_grid to coords.
 * Should be used instead of the division operator. */
coords_t coords_grid_to_coords(coords_grid_t coords_grid, unsigned int resolution);

#endif /* HEADER_BASICS_ */
