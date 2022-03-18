
""" Note:
OpenCV's SIFT and SURF implementations must be available for this to work.
The Anaconda environment will install OpenCV with these implemnations available
(which is not the default since OpenCV 3,
and requires a painful config and build process to get these manually).

https://www.anaconda.com/
"""

import cv2

input_image = cv2.imread("../../impl/pics/popeye.jpg")
sift = cv2.SIFT_create()
keypoints, keypoint_descriptors = sift.detectAndCompute(input_image, None)

result_image = None
result_image = cv2.drawKeypoints(input_image, keypoints, result_image)
cv2.imwrite("sift_keypoints.png", result_image)

output = open("keypoints", "w")
for keypoint in keypoints:
	output.write(f"{keypoint.pt[0]}, {keypoint.pt[1]}\n")
output.close()
