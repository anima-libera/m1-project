
import os
import subprocess
import random
import matplotlib.pyplot as plt

os.chdir("bin")

TIMEOUT = 110
ERROR_MAX = 0.49 #0.36

some_good_param_value_tables = [
	[
		1.0, 0.0, 0.0, 0.0, 0.0, 0.0001,
		4000, 400, 0.6, 2, 200
	],
	[
		1.0, 0.0, 0.0, 0.0, 0.0, 0.0001,
		4000, 200, 0.3, 4, 256
	]
]

initial_param_space_table = [ # [min, max, is_int]
	[-5.0, 5.0, False],
	[-5.0, 5.0, False],
	[-5.0, 5.0, False],
	[-5.0, 5.0, False],
	[-5.0, 5.0, False],
	[-5.0, 5.0, False],
	[1, 10000, True],
	[1, 1000, True],
	[0.01, 1.0, False],
	[1, 4, True],
	[2, 512, True],
]

param_space_table = initial_param_space_table.copy()

def param_table_is_correct(param_value_table, param_space_table):
	for value, space in zip(param_value_table, param_space_table):
		if not (space[0] <= value <= space[1]):
			return False
		assert type(value) == (int if space[2] else float)
	if not (param_value_table[7] <= param_value_table[6]):
		return False
	return True

def random_param_table(param_space_table):
	def random_param_table_maybe_incorrect(param_space_table):
		return tuple(map(
			lambda space: (random.randint if space[2] else random.uniform)(space[0], space[1]),
			param_space_table))
	param_value_table = random_param_table_maybe_incorrect(param_space_table)
	while not param_table_is_correct(param_value_table, param_space_table):
		param_value_table = random_param_table_maybe_incorrect(param_space_table)
	return param_value_table

def mutate_param_table(param_value_table, param_space_table):
	assert param_table_is_correct(param_value_table, param_space_table)
	def mutate_param_table_maybe_incorrect(param_value_table, param_space_table):
		change = random.randint(2, 10)
		return tuple(
			param_value_table[i] if random.randint(1, change) == 1 else
				(param_value_table[i] + random.randint(-5, 5)
				if param_space_table[i][2]
				else param_value_table[i] + random.uniform(
					-(param_space_table[i][1] - param_space_table[i][0]) * 0.1,
					+(param_space_table[i][1] - param_space_table[i][0]) * 0.1))
			for i in range(len(param_value_table))
		)
	new_param_value_table = mutate_param_table_maybe_incorrect(
		param_value_table, param_space_table)
	while ((not param_table_is_correct(new_param_value_table, param_space_table))
		or new_param_value_table == param_value_table):
		new_param_value_table = mutate_param_table_maybe_incorrect(
			param_value_table, param_space_table)
	return new_param_value_table

# result = (time, error, line_count) or None in case of timeout
def stringart(param_value_table):
	def command(param_value_table):
		return ("./implbin",
			"popeye",
			"--coefs", *map(str, param_value_table[:6]),
			"--params", *map(str, param_value_table[6:]),
			"--print-time-and-error",
			"--no-output")
	try:
		output = subprocess.check_output(
			command(param_value_table), timeout=TIMEOUT).decode("ascii").splitlines()[-1].split()
		time = float(output[0])
		error = float(output[1])
		line_count = int(output[2])
		return (time, error, line_count)
	except subprocess.TimeoutExpired:
		return None

def best_output(a, b):
	if a[0] < b[0] and a[1] < b[1]:
		return a
	elif a[0] > b[0] and a[1] > b[1]:
		return b
	else:
		return None

last_results = dict()
try:
	last_results = eval(open("results", "r").read())
	if len(last_results) >= 1:
		print(f"Will load the {len(last_results)} last results first")
except Exception as e:
	print(e)

input_output_table = dict()
best_points = set()
try:
	while True:

		if len(last_results) == 0:
			points = tuple(filter(lambda x: x != None, input_output_table.values()))
			times = tuple(map(lambda v: v[0], points))
			errors = tuple(map(lambda v: v[1], points))
			best_points_values = tuple(map(lambda x: input_output_table[x], best_points))
			best_times = tuple(map(lambda v: v[0], best_points_values))
			best_errors = tuple(map(lambda v: v[1], best_points_values))

			plt.axis([0, TIMEOUT, 0, ERROR_MAX])
			plt.scatter(times, errors, c="orange")
			plt.scatter(best_times, best_errors, c="black")
			plt.savefig("search_front.png", dpi = 100)

			plt.axis([0, 100, 0, 0.12])
			plt.scatter(times, errors, c="orange")
			plt.scatter(best_times, best_errors, c="black")
			plt.savefig("search_front_zoom.png", dpi = 100)

		if len(last_results) >= 1:
			some_last_result = last_results.popitem()
			if len(last_results) == 0:
				print("Loading the last previous result")
			param_value_table = some_last_result[0]
			output = some_last_result[1]
			time = output[0] if output != None else TIMEOUT

		else:
			if tuple(some_good_param_value_tables[0]) not in input_output_table.keys():
				param_value_table = tuple(some_good_param_value_tables[0])
			elif tuple(some_good_param_value_tables[1]) not in input_output_table.keys():
				param_value_table = tuple(some_good_param_value_tables[1])

			elif random.randint(1, 100) <= 50 and len(best_points) >= 1:
				min_error = 10 * ERROR_MAX
				min_error_best_point = None
				for best_point in best_points:
					if input_output_table[best_point][1] < min_error:
						min_error = input_output_table[best_point][1]
						min_error_best_point = best_point
				best_point_to_mutate = (min_error_best_point if min_error_best_point != None
					else random.choice(tuple(best_points)),)[0]
				param_value_table = mutate_param_table(best_point_to_mutate, param_space_table)
			elif random.randint(1, 100) <= 70 and len(best_points) >= 1:
				best_point_to_mutate = random.choice(tuple(best_points))
				param_value_table = mutate_param_table(best_point_to_mutate, param_space_table)
			else:
				param_value_table = random_param_table(param_space_table)

			time = TIMEOUT
			output = stringart(param_value_table)
		
		count = len(input_output_table)
		if output != None:
			time = output[0]
			if output[1] > ERROR_MAX:
				output = None
				print(f"[{count:06}] ({time}) error too high")
			else:
				print(f"[{count:06}] {output}")
		else:
			print(f"[{count:06}] timeout")
		input_output_table[param_value_table] = output

		if output != None:
			is_best_point = True
			beaten_points = set()
			for point in best_points:
				point_output = input_output_table[point]
				best = best_output(output, point_output)
				if best == point_output:
					is_best_point = False
					break
				elif best == output:
					beaten_points.add(point)
			if is_best_point:
				best_points.difference_update(beaten_points)
				best_points.add(param_value_table)
				print("New best points:")
				for best_point in best_points:
					print(input_output_table[best_point],
						"--coefs", *map(str, best_point[:6]),
						"--params", *map(str, best_point[6:]))
				
				if len(last_results) == 0:
					results = open("results", "w")
					results.write(repr(input_output_table))
					results.close()
					print("Results saved")
		
		if random.randint(1, 20) == 1:
			if len(last_results) == 0:
				results = open("results", "w")
				results.write(repr(input_output_table))
				results.close()
				print("Results saved")

except KeyboardInterrupt:
	if len(last_results) == 0:
		print()
		results = open("results", "w")
		results.write(repr(input_output_table))
		results.close()
		print("Results saved")


os.chdir("..")
