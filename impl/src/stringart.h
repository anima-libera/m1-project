
/* String art algorithms. */

#ifndef HEADER_STRINGART_
#define HEADER_STRINGART_

#include "canvas.h"
#include "pinset.h"
#include "random.h"

enum error_formula_enum_t
{
	ERROR_FORMULA_DIFF,
	ERROR_FORMULA_DIFF_SQUARE,
};
typedef enum error_formula_enum_t error_formula_enum_t;

enum score_formula_enum_t
{
	SCORE_FORMULA_DIFF_AVG_GS_ERASE_TARGET,
};
typedef enum score_formula_enum_t score_formula_enum_t;

struct string_art_input_t
{
	canvas_float_t input_canvas;
	canvas_float_t importance_canvas;
	float current_canvas_background_gs;
	gs_op_t line_color;
	error_formula_enum_t error_formula;
	score_formula_enum_t score_formula;
	unsigned int resolution_factor;
	pinset_t pinset;
	unsigned int line_pool_length;
	unsigned int iteration_max_number;
};
typedef struct string_art_input_t string_art_input_t;

/* TODO: Make the string art algorithm state (between iterations) representable by a struct, 
 * and make that struct serializable! */

struct string_art_state_t
{
	canvas_float_t target_canvas;
	canvas_float_t target_erase_canvas;
	float current_canvas_background_gs;
	gs_op_t line_color;
	error_formula_enum_t error_formula;
	unsigned int resolution_factor;
	pinset_t pinset;
	unsigned int line_pool_length;
	unsigned int iteration_max_number;
	canvas_gs_op_t current_canvas_sd;
	canvas_gs_op_t current_canvas_hd;
	rg_t rg;
};
typedef struct string_art_state_t string_art_state_t;

void perform_string_art(string_art_input_t input);

#endif /* HEADER_STRINGART_ */
