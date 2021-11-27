
#include "bmp.h"
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

void output_bmp(bmp_pixel_grid_t pixel_grid, const char* output_file_path)
{
	FILE* output_file = fopen(output_file_path, "wb");
	assert(output_file != NULL);

	static_assert(sizeof(bmp_pixel_t) == 4, "bug");

	#define MAGIC_NUMBER_SIZE 2
	#define HEADER_SIZE (sizeof(bmp_header_t))
	#define CHANNEL_MASKS_SIZE (3 * sizeof(bmp_pixel_t))
	#define IMAGE_SIZE (pixel_grid.w * pixel_grid.h * sizeof(bmp_pixel_t))

	/* Magic number. */
	fwrite("B", 1, 1, output_file);
	fwrite("M", 1, 1, output_file);

	/* Header (using the BITMAPINFOHEADER header version). */
	const bmp_header_t header = {
		.file_size =
			MAGIC_NUMBER_SIZE + HEADER_SIZE + CHANNEL_MASKS_SIZE + IMAGE_SIZE,
		.file_offset_to_pixel_array =
			MAGIC_NUMBER_SIZE + HEADER_SIZE + CHANNEL_MASKS_SIZE,
		.dbi_header_size =
			sizeof(bmp_header_t) - offsetof(bmp_header_t, dbi_header_size),
		.image_width = pixel_grid.w,
		.image_height = pixel_grid.h,
		.planes = 1,
		.bits_per_pixel = sizeof(bmp_pixel_t) * 8,
		.compression = 3, /* BI_BITFIELDS */
		.image_size = pixel_grid.w * pixel_grid.h * sizeof(bmp_pixel_t),
		.x_pixel_per_meter = 5000, /* Is it good ? It seems to be ok... */
		.y_pixel_per_meter = 5000, /* Is it good ? It seems to be ok... */
		.colors_in_color_table = 0, /* For some reason it must be 0, even though it is 3. */
		.important_color_count = 0,
	};
	fwrite(&header, sizeof header, 1, output_file);
	/* Note:
	 * BI_BITFIELDS is used here instead of BI_ALPHABITFIELDS due ot a lack
	 * of support for the latter option. */

	bmp_pixel_t mask;
	fwrite((mask = (bmp_pixel_t){.r = 255}, &mask), sizeof(bmp_pixel_t), 1, output_file);
	fwrite((mask = (bmp_pixel_t){.g = 255}, &mask), sizeof(bmp_pixel_t), 1, output_file);
	fwrite((mask = (bmp_pixel_t){.b = 255}, &mask), sizeof(bmp_pixel_t), 1, output_file);

	/* Pixel array. As the size of a pixel is 32 bits, the padding between rows
	 * of the bitmap pixel array to alling on 32 bits is of size 0
	 * and can be ignored. */
	fwrite(pixel_grid.grid, sizeof(bmp_pixel_t), pixel_grid.w * pixel_grid.h, output_file);

	fclose(output_file);
}
