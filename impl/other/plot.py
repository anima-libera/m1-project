
import matplotlib.pyplot as plt

error_values = list(map(float, open("error", "r").readlines()))

plt.plot(error_values)
plt.show()
