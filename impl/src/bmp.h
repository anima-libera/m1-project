
/* Bitmap image file generation. */

#ifndef HEADER_BMP_
#define HEADER_BMP_

#include <stdint.h>
#include "pg.h"

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
	uint32_t compression; /* A 0 means no compression (thus probably RGBA). */
	uint32_t image_size;
	int32_t x_pixel_per_meter;
	int32_t y_pixel_per_meter;
	uint32_t colors_in_color_table; /* Can be 0. */
	uint32_t important_color_count; /* Can be 0. */
};
typedef struct bmp_header_t bmp_header_t;
/* Note: It appears that the supposed alpha channel doesn't allow for
 * transparency in the output bitmap image, which may implie that the
 * designated pixel format (in the compression member and with the
 * bits_per_pixel member) is actually RGBX and not RGBA. */
/* TODO: Make the alpha channel actually the alpha channel and not
 * just padding, maybe by using BITMAPV3INFOHEADER header ? */

/* Creates a bitmap image file at the given path, containing the image
 * described by the given pixel grid of the given size. */
void output_pg_as_bitmap(pg_t pg, const char* output_file_path);

#endif /* HEADER_BMP_ */
