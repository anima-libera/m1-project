
#include "pg.h"
#include <stdint.h>
#include <stddef.h> /* offsetof */
#include <limits.h>
#include <stdio.h>
#include <assert.h>

static_assert(CHAR_BIT == 8, "non supported wierd implementation");

/* Bitmap file format header (without the magic number),
 * https://en.wikipedia.org/wiki/BMP_file_format
 * https://www.digicamsoft.com/bmp/bmp.html
 * for more. */
struct bmp_header_t
{
	/* BITMAPFILEHEADER members (without the magic number). */
	/* The magic number 'BM' should be written just before this header,
	 * without padding (that is why it is not a member of this struct type). */
	uint32_t file_size;
	uint16_t reserved_1, reserved_2; /* Can both be 0. */
	uint32_t file_offset_to_pixel_array;

	/* BITMAPINFOHEADER members. */
	uint32_t dbi_header_size; /* Size of the BITMAPINFOHEADER header only. */
	int32_t image_width;
	int32_t image_height;
	uint16_t planes; /* Must be 1. */
	uint16_t bits_per_pixel;
	uint32_t compression; /* 0 means no compression, 3 means BI_BITFIELDS. */
	uint32_t image_size;
	int32_t x_pixel_per_meter;
	int32_t y_pixel_per_meter;
	uint32_t colors_in_color_table; /* Can be 0. */
	uint32_t important_color_count; /* Can be 0. */
};
typedef struct bmp_header_t bmp_header_t;

void output_pg_as_bitmap(pg_t pg, const char* output_file_path)
{
	FILE* output_file = fopen(output_file_path, "wb");
	assert(output_file != NULL);

	#define MAGIC_NUMBER_SIZE 2
	#define HEADER_SIZE (sizeof(bmp_header_t))
	#define CHANNEL_MASKS_SIZE (3 * 4)
	#define IMAGE_SIZE (pg.w * pg.h * 4)

	/* Magic number. */
	fwrite("B", 1, 1, output_file);
	fwrite("M", 1, 1, output_file);

	/* Header (using the BITMAPINFOHEADER header version). */
	bmp_header_t header = {
		.file_size =
			MAGIC_NUMBER_SIZE + HEADER_SIZE + CHANNEL_MASKS_SIZE + IMAGE_SIZE,
		.file_offset_to_pixel_array =
			MAGIC_NUMBER_SIZE + HEADER_SIZE + CHANNEL_MASKS_SIZE,
		.dbi_header_size =
			sizeof(bmp_header_t) - offsetof(bmp_header_t, dbi_header_size),
		.image_width = pg.w,
		.image_height = pg.h,
		.planes = 1,
		.bits_per_pixel = sizeof(pixel_t) * 8,
		.compression = 3, /* BI_BITFIELDS */
		.image_size = pg.w * pg.h * 4,
		.x_pixel_per_meter = 5000, /* Is it good ? It seems to be ok... */
		.y_pixel_per_meter = 5000, /* Is it good ? It seems to be ok... */
		.colors_in_color_table = 0, /* Must be 0, even though it is 3. */
		.important_color_count = 0,
	};
	fwrite(&header, sizeof header, 1, output_file);
	/* Note:
	 * BI_BITFIELDS is used here instead of BI_ALPHABITFIELDS due ot a lack
	 * of support of the latter option. */

	pixel_t mask;
	fwrite((mask = (pixel_t){.r = 255}, &mask), 4, 1, output_file);
	fwrite((mask = (pixel_t){.g = 255}, &mask), 4, 1, output_file);
	fwrite((mask = (pixel_t){.b = 255}, &mask), 4, 1, output_file);

	/* Pixel array. As the size of a pixel is 32 bits, padding between rows
	 * of the bitmap pixel array to alling on 32 bits is of size 0
	 * and can be ignored. */
	fwrite(pg.pixel_grid, pg.w * pg.h * 4, 1, output_file);

	fclose(output_file);
}
