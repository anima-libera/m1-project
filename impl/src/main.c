
#include "canvas.h"
#include "line.h"
#include "pinset.h"
#include <stdlib.h>
#include <stdio.h>

int main(void)
{
	canvas_gs_op_t canvas_sd;
	canvas_sd.resolution = 128;
	canvas_sd.grid = malloc(canvas_sd.resolution * canvas_sd.resolution * sizeof(gs_op_t));

	canvas_gs_op_t canvas_hd;
	canvas_hd.resolution = 128 * 4;
	canvas_hd.grid = malloc(canvas_hd.resolution * canvas_hd.resolution * sizeof(gs_op_t));

	pinset_t pinset = pinset_generate_circle(30);

	for (unsigned int i = 0; i < pinset.pin_count; i++)
	for (unsigned int j = i + 1; j < pinset.pin_count; j++)
	{
		line_pixels_t line_pixels_hd = line_coords_to_line_pixels(
			(line_coords_t){
				.coords_a = pinset.pin_array[i],
				.coords_b = pinset.pin_array[j],
				.color = {.gs = 1.0f, .op = 1.0f}},
			canvas_hd.resolution,
			line_plot_pixels_mid_point);

		line_pixels_t line_pixels_sd = line_pixels_downscale_hd_to_sd(line_pixels_hd,
			canvas_sd.resolution);

		canvas_gs_op_draw_line_pixels(canvas_hd, line_pixels_hd);
		canvas_gs_op_draw_line_pixels(canvas_sd, line_pixels_sd);

		line_pixels_cleanup(line_pixels_hd);
		line_pixels_cleanup(line_pixels_sd);
	}

	canvas_gs_op_output_bmp(canvas_hd, 0.0f, "test_hd.bmp");
	canvas_gs_op_output_bmp(canvas_sd, 0.0f, "test_sd.bmp");

	return 0;
}
