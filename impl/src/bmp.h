
/* Bitmap image file generation. */

#ifndef HEADER_BMP_
#define HEADER_BMP_

#include <stdint.h>

struct bmp_pixel_t
{
	unsigned char r, g, b, a;
};
typedef struct bmp_pixel_t bmp_pixel_t;

struct bmp_pixel_grid_t
{
	unsigned int w, h;
	bmp_pixel_t* grid;
};
typedef struct bmp_pixel_grid_t bmp_pixel_grid_t;

/* Creates or overwrites a bitmap image file at the given path,
 * containing the image described by the given pixel grid of the given size. */
void output_bmp(bmp_pixel_grid_t pixel_grid, const char* output_file_path);

#endif /* HEADER_BMP_ */
