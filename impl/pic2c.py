
import sys

#true_stderr = sys.stderr
#sys.stderr = open("/dev/null", "a")
import matplotlib.pyplot as plt
#sys.stderr = true_stderr

pic_name = "".join(sys.argv[1].split(".")[:-1])
pic_filepath = f"pics/{sys.argv[1]}"
print(pic_filepath)
gen_filepath = f"rawpics/{pic_name}.raw"
dim_filepath = f"rawpics/{pic_name}.dim"

pic = plt.imread(pic_filepath)
h, w = pic.shape[:2]
color_format = pic.shape[2] if len(pic.shape) >= 3 else 1
print(w, h, color_format)

raw_data = []
for y in range(h):
	for x in range(w):
		if color_format == 1:
			pixel = pic[h - y - 1, x]
			raw_data.append(pixel)
			raw_data.append(pixel)
			raw_data.append(pixel)
			raw_data.append(255)
		else:
			pixel = pic[h - y - 1, x, 0:3]
			raw_data.append(pixel[0])
			raw_data.append(pixel[1])
			raw_data.append(pixel[2])
			raw_data.append(255)
	if y % 50 == 0:
		print(f"{(y+1):4} / {h:4}")
print(f"{h:4} / {h:4}")
print("done ^^")

with open(gen_filepath, "wb") as dst_file:
	dst_file.write(bytes(raw_data))

with open(dim_filepath, "w") as dst_file:
	dst_file.write(f"{w} {h}\n")
