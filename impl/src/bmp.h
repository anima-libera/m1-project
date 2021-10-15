
/* Bitmap image file generation. */

#ifndef HEADER_BMP_
#define HEADER_BMP_

#include "pg.h"

/* Creates a bitmap image file at the given path, containing the image
 * described by the given pixel grid of the given size. */
void output_pg_as_bitmap(pg_t pg, const char* output_file_path);

#endif /* HEADER_BMP_ */
