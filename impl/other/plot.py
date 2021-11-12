
import sys
import tkinter

#true_stderr = sys.stderr
#sys.stderr = open("/dev/null", "a")
import matplotlib.pyplot as plt
#sys.stderr = true_stderr

if __name__ == "__main__":
	data_log_filepath = "data_log" if len(sys.argv) <= 1 else sys.argv[1]
else:
	data_log_filepath = "bin/data_log"

error_delta_array = []
error_new_array = []
error_sq_delta_array = []
error_sq_new_array = []
line_minimal_radius_array = []
average_grayscale_array = []
average_grayscale_hd_array = []
error_cavnas_input_array = []
error_sq_cavnas_input_array = []

with open(data_log_filepath, "r") as data_log:
	lines = list(map(str.rstrip, data_log.readlines()))
	const_data = lines[0].split(" ")

	input_average_grayscale = float(const_data[0])

	assert lines[1] == ""

	for line in lines[2:]:
		iter_data = line.split(" ")

		i = 0
		error_delta_array.append(float(iter_data[i])); i += 1
		error_new_array.append(float(iter_data[i])); i += 1
		error_sq_delta_array.append(float(iter_data[i])); i += 1
		error_sq_new_array.append(float(iter_data[i])); i += 1
		line_minimal_radius_array.append(float(iter_data[i])); i += 1
		average_grayscale_array.append(float(iter_data[i])); i += 1
		average_grayscale_hd_array.append(float(iter_data[i])); i += 1
		error_cavnas_input_array.append(float(iter_data[i])); i += 1
		error_sq_cavnas_input_array.append(float(iter_data[i])); i += 1

x_array = list(range(1, len(error_delta_array)+1))

average_grayscale_crossing = (len(error_delta_array), input_average_grayscale)
for x, av_gray in zip(x_array, average_grayscale_array):
	if av_gray > input_average_grayscale:
		average_grayscale_crossing = (x, av_gray)
		break

average_grayscale_hd_crossing = (len(error_delta_array), input_average_grayscale)
for x, av_gray_hd in zip(x_array, average_grayscale_hd_array):
	if av_gray_hd > input_average_grayscale:
		average_grayscale_hd_crossing = (x, av_gray_hd)
		break

title = "Canvas evolution over iterations"

plot_count = 5

i = 1
plt.subplot(plot_count, 1, i); i += 1
plt.plot(x_array, error_sq_delta_array, "blue")
plt.plot(x_array, error_delta_array, "red")
plt.title(title)
plt.ylabel("Err delta")

plt.subplot(plot_count, 1, i); i += 1
plt.plot(x_array, error_sq_new_array, "blue")
plt.plot(x_array, error_new_array, "green")
plt.ylabel("Err new")

plt.subplot(plot_count, 1, i); i += 1
plt.plot(x_array, line_minimal_radius_array, "orange")
plt.ylabel("Min radius")

plt.subplot(plot_count, 1, i); i += 1
plt.plot(x_array, average_grayscale_array, "black")
plt.plot(x_array, average_grayscale_hd_array, "gray")
plt.plot(*average_grayscale_crossing,
	marker = "o", markersize = 6, markerfacecolor = "black", markeredgecolor = "green")
plt.plot(*average_grayscale_hd_crossing,
	marker = "o", markersize = 6, markerfacecolor = "gray", markeredgecolor = "green")
plt.ylabel("Avg grayscale")

plt.subplot(plot_count, 1, i); i += 1
plt.plot(x_array, error_sq_cavnas_input_array, "blue")
plt.plot(x_array, error_cavnas_input_array, "purple")
plt.ylabel("Err can-inp")

plt.xlabel("Iterations")
assert i == plot_count+1

fig = plt.gcf()
fig.set_size_inches(8, 8)

def plot_to_file(filepath):
	global plt
	plt.savefig(filepath, dpi = 100)

def plot_to_screen():
	global plt
	plt.show()

if __name__ == "__main__":
	plot_to_screen()
