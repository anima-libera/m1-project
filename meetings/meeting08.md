
# Meeting 8 - 26/11/2021

## What has been done

### Refactoring

As explained in the previous meeting report, almost all the code is being refactored (rewritten from scratch to be exact) with better abstractions in mind. It is hard to quantify the amount of code that has been reworked in comparison with what the code was before the beginning of this refactoring.

### New features due to refactoring

Non-opaque strings of any grayscale color are now supported. Canvas with transparent areas are now supported. Non-white canvas backgrounds are now supported.

## What to do next

### Continue the refactoring until complete

Work in progress...

### Go through the TODO lists of the last meetings

There are still ideas left there.

### Big downscale for error calculation

It taking into account an error calculation on pixels of a greatly downscaled version of a line compared to a equally greatly downscaled version of the input picture, and if this error calculaion squares the distances between pixel colors, then we hope that it will make dark lines in white areas spread (to avoid passing through the same big pixel too much) which may be a good behavior.
