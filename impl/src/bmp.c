
#include <stddef.h> /* offsetof */
#include <stdio.h>
#include <assert.h>
#include "bmp.h"
#include "pg.h"

void output_pg_as_bitmap(pg_t pg, const char* output_file_path)
{
	FILE* output_file = fopen(output_file_path, "wb");
	assert(output_file != NULL);

	/* Magic number. */
	fwrite("B", 1, 1, output_file);
	fwrite("M", 1, 1, output_file);

	/* Header (using the BITMAPINFOHEADER header version). */
	bmp_header_t header = {
		.file_size = 2 + sizeof(bmp_header_t) + pg.w * pg.h * 4,
		.file_offset_to_pixel_array = 2 + sizeof(bmp_header_t),
		.dbi_header_size =
			sizeof(bmp_header_t) - offsetof(bmp_header_t, dbi_header_size),
		.image_width = pg.w,
		.image_height = pg.h,
		.planes = 1,
		.bits_per_pixel = 32,
		.compression = 0,
		.image_size = pg.w * pg.h * 4,
		.x_pixel_per_meter = 5000 /* Is it good ? */,
		.y_pixel_per_meter = 5000 /* Is it good ? */,
		.colors_in_color_table = 0,
		.important_color_count = 0,
	};
	fwrite(&header, sizeof header, 1, output_file);

	/* Pixel array. As the size of a pixel is 32 bits, padding between rows
	 * of the bitmap pixel array to alling on 32 bits is of size 0
	 * and can be ignored. */
	fwrite(pg.pixel_grid, pg.w * pg.h * 4, 1, output_file);

	fclose(output_file);
}
