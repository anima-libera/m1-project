
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
	SCORE_FORMULA_DIFF_AVG_GS_TARGET,
	SCORE_FORMULA_NEG_ERROR_DELTA,
	SCORE_FORMULA_NEG_ERROR_DELTA_PLUS_DELTA_SD,
	SCORE_FORMULA_NEG_ERROR_DELTA_IF_DELTA_SD,
	SCORE_FORMULA_NEG_ERROR_DELTA_NORMALIZED,
	SCORE_FORMULA_NEG_ERROR_DELTA_POSITIVE_ERROR_PENALTY,
	SCORE_FORMULA_DIFF_AVG_GS_ERASE_TARGET_POSITIVE_ERROR_PENALTY,
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
	unsigned int line_number_per_iteration;
	unsigned int iteration_max_number;
};
typedef struct string_art_input_t string_art_input_t;

struct line_stats_t
{
	float error_delta_sd;
	float positive_error_delta_sd; /* Too low, use positive_error_delta_hd_corrected. */
	float error_delta_hd_corrected;
	float positive_error_delta_hd_corrected;
	float delta_sd;
	float delta_hd_corrected;
	float avg_gs_erase_target;
	float avg_gs_target;
	float score;
};
typedef struct line_stats_t line_stats_t;

struct iteration_stats_t
{
	unsigned int line_index_lower;
	unsigned int line_number;
};
typedef struct iteration_stats_t iteration_stats_t;

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
	unsigned int line_number_per_iteration;
	unsigned int iteration_max_number;
	canvas_gs_op_t current_canvas_sd;
	canvas_gs_op_t current_canvas_hd;
	rg_t rg;
};
typedef struct string_art_state_t string_art_state_t;

void perform_string_art(string_art_input_t input);

#endif /* HEADER_STRINGART_ */
