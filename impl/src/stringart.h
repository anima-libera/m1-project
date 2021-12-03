
/* String art algorithms. */

#ifndef HEADER_STRINGART_
#define HEADER_STRINGART_

#include "canvas.h"
#include "pinset.h"

enum error_formula_enum_t
{
	ERROR_FORMULA_DIFF,
	ERROR_FORMULA_DIFF_SQUARE,
};
typedef enum error_formula_enum_t error_formula_enum_t;

struct string_art_input_t
{
	canvas_float_t target_canvas;
	float current_canvas_background_gs;
	gs_op_t line_color;
	error_formula_enum_t error_formula;
	unsigned int resolution_factor;
	pinset_t pinset;
	unsigned int line_pool_length;
	unsigned int iteration_max_number;
};
typedef struct string_art_input_t string_art_input_t;

void perform_string_art(string_art_input_t input);

#endif /* HEADER_STRINGART_ */
