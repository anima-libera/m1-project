
import matplotlib.pyplot as plt

pic_name = "bird"
pic_filepath = f"pics/{pic_name}.jpg"
gen_filepath = f"rawpics/{pic_name}.raw"

pic = plt.imread(pic_filepath)
w, h, color_format = pic.shape
print(w, h, color_format)
assert color_format >= 3

raw_data = []
for y in range(h):
	for x in range(w):
		pixel = pic[x, y, 0:3]
		raw_data.append(pixel[0])
		raw_data.append(pixel[1])
		raw_data.append(pixel[2])
		raw_data.append(255)
	if y % 50 == 0:
		print(f"{(y+1):4} / {h:4}")
print("done ^^")

with open(gen_filepath, "wb") as dst_file:
	dst_file.write(bytes(raw_data))
