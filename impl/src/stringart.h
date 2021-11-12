
/* String art algorithms. */

#ifndef HEADER_STRINGART_
#define HEADER_STRINGART_

#include "pg.h"
#include "pinset.h"
#include "random.h"
#include "line.h"
#include <stdint.h>

typedef struct stringart_mem_t stringart_mem_t;

#if 0
struct line_t
{
	float xa, xb, ya, yb;
	pixel_t color;
};
typedef struct line_t line_t;
#endif

struct line_data_t
{
	line_t line;
	int pin_a;
	int pin_b;
	float error_delta;
	float error_new;
	float error_sq_delta;
	float error_sq_new;
};
typedef struct line_data_t line_data_t;

typedef unsigned int (*line_pool_generator_t)(stringart_mem_t* mem,
	line_data_t* line_data_pool, unsigned int line_data_pool_len);
typedef float (*line_score_computer_t)(stringart_mem_t* mem,
	line_data_t* line_data);
typedef int (*winning_line_handler_t)(stringart_mem_t* mem,
	line_data_t* line_data);

struct stringart_input_t
{
	pinset_t pinset;
	unsigned int line_colorset_len;
	pixel_t* line_colorset;
	pg_t input;
	unsigned int hd_sd_ratio;
	uint32_t rg_seed;
};
typedef struct stringart_input_t stringart_input_t;

struct stringart_algo_t
{
	unsigned int final_line_number_max;
	unsigned int line_data_pool_len_max;
	line_pool_generator_t line_pool_generator;
	line_score_computer_t line_score_computer;
	winning_line_handler_t winning_line_handler;
	int halt_on_average_grayscale_crossing;
	int halt_on_average_grayscale_hd_crossing;
	unsigned int halt_on_error_stagnation_length;
};
typedef struct stringart_algo_t stringart_algo_t;

struct stringart_state_t
{
	unsigned int iteration; /* Current iteration index. */
	pg_t canvas;
	pg_t canvas_hd;
	pg_t target;
	rg_t rg;
};
typedef struct stringart_state_t stringart_state_t;

struct stringart_logdata_t
{
	pg_t trace;
	float* error_delta_array;
	float* error_new_array;
	float* error_sq_delta_array;
	float* error_sq_new_array;
	float* line_minimal_radius_array;
	float input_average_grayscale;
	float* average_grayscale_array;
	float* average_grayscale_hd_array;
	float* error_cavnas_input_array;
	float* error_sq_cavnas_input_array;
};
typedef struct stringart_logdata_t stringart_logdata_t;

struct stringart_mem_t
{
	stringart_input_t input;
	stringart_algo_t algo;
	stringart_state_t state;
	stringart_logdata_t logdata;
};
typedef struct stringart_mem_t stringart_mem_t;

void stringart_perform_algo(stringart_mem_t* mem);

unsigned int linepoolgen_random(stringart_mem_t* mem,
	line_data_t* line_data_pool, unsigned int line_data_pool_len);
unsigned int linepoolgen_iter_random(stringart_mem_t* mem,
	line_data_t* line_data_pool, unsigned int line_data_pool_len);

float linescorecomp_error_new(stringart_mem_t* mem,
	line_data_t* line_data);
float linescorecomp_error_delta(stringart_mem_t* mem,
	line_data_t* line_data);
float linescorecomp_error_sq_new(stringart_mem_t* mem,
	line_data_t* line_data);
float linescorecomp_error_sq_delta(stringart_mem_t* mem,
	line_data_t* line_data);

int winlinehand_draw_erase_log_print(stringart_mem_t* mem,
	line_data_t* line_data);
int winlinehand_draw_log_print(stringart_mem_t* mem,
	line_data_t* line_data);

#endif /* HEADER_STRINGART_ */
