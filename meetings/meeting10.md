
# Meeting 10 - 15/10/2021

## What has been done

### Debugging

Fixed a rendering bug with a workaround. The problem was that for some resolutions, floating points errors would make conversions between different representations of coordinates non-reversible. This was fixed by trying different formulats by hand, and automating the insertion of a margin on the sides of an input picture to get it to a resolution that does not have problems.

### Importance layer

Added the fact that the string art algorithm now takes into account an importance field. It allows to make some areas of the input more important than others, which will make the algorithm "sacrifice" a bit of the low importance areas to focus more on the more important ones.

### Erasing lines from target

Re-implemented the technique that consists in erasing a winning line from the target, as it still gives better results than everything else that was attempted util now. It still behaves well for a black background and white lines, but it seems like there is a mistake in the formula.

### Pinsets

Added pin sets generators to see how they would perform. The totally random pinset generator is intresting, the square and the circle one are still better, the one with a circle and its center doesn a good job on Popeye, the X-shaped one is useless.

## What to do next

### Intrest points

Use intrest points to automate importance field creation.

- Algorithms: Harris or SURF.
- [https://opencv24-python-tutorials.readthedocs.io/en/latest/py_tutorials/py_feature2d/py_surf_intro/py_surf_intro.html]
- [https://opencv24-python-tutorials.readthedocs.io/en/latest/py_tutorials/py_feature2d/py_table_of_contents_feature2d/py_table_of_contents_feature2d.html]

### Erasing formula

Fix the mistake in the erasing formula.

### Other pinset generator

Use algorithms that turn images into dots to generate a pinset with pins in dark areas.

### Previous TODO lists

Go through previous TODO lists as well (as always ^^').
