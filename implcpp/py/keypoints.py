
"""
Generates keypoint list via the OpenCV SURF implementation,
so that they can be used as pin positions for string art.
"""

import cv2
import sys

image = cv2.imread(sys.argv[0], 0)

surf = cv2.xfeatures2d.SURF_create() # Doesn't work as-is, needs a custom compilation of OpenCV.
# Note: Have not succedded yet in compiling and installing OpenCV with patents.
surf.upright = True

keypoints = surf.detect(image, None)

for keypoint in keypoints:
	print(keypoint)
