
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
average_grayscale_array = []
error_cavnas_input_array = []

with open(data_log_filepath, "r") as data_log:
	lines = list(map(str.rstrip, data_log.readlines()))
	const_data = lines[0].split(" ")

	input_average_grayscale = float(const_data[0])

	assert lines[1] == ""

	for line in lines[2:]:
		iter_data = line.split(" ")

		error_delta = float(iter_data[0])
		error_new = float(iter_data[1])
		average_grayscale = float(iter_data[2])
		error_cavnas_input = float(iter_data[3])

		error_delta_array.append(error_delta)
		error_new_array.append(error_new)
		average_grayscale_array.append(average_grayscale)
		error_cavnas_input_array.append(error_cavnas_input)

x_array = list(range(1, len(error_delta_array)+1))

average_grayscale_crossing = (len(error_delta_array), input_average_grayscale)
for x, av_gray in zip(x_array, average_grayscale_array):
	if av_gray > input_average_grayscale:
		average_grayscale_crossing = (x, av_gray)
		break

title = "Canvas evolution over iterations"

plt.subplot(4, 1, 1)
plt.plot(x_array, error_delta_array, "red")
plt.title(title)
plt.ylabel("Error delta")

plt.subplot(4, 1, 2)
plt.plot(x_array, error_new_array, "green")
plt.ylabel("Error new")

plt.subplot(4, 1, 3)
plt.plot(x_array, average_grayscale_array, "blue")
plt.plot(*average_grayscale_crossing, marker="o", markersize=6, markerfacecolor="red")
plt.ylabel("Average grayscale")

plt.subplot(4, 1, 4)
plt.plot(x_array, error_cavnas_input_array, "purple")
plt.ylabel("Error canvas-input")

plt.xlabel("Iterations")

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
