
# Meeting 5 - 22/10/2021

## What has been done

### Big code refactoring

String art algorithm, input, memory state and data monitored during the many iterations are now stored in a struct type that make it easy to tweak the now modular parts of the algorithm and the input.

### Grayscale pictures

We are back to grayscale (for now). Grayscale pictures are now supported in the import pipeline, and color pictures are grayscalized in the C program.

### Data monitoring and analysis

Each iteration of the process (each line drawn), the canvas and target are modified, and this modification is "recorded" in trems of various error computations and other data (average grayscale for example) (it is the only other "various" data for now). These are then plotted on a graph via a python script that uses matplotlib as the plotting engine.

Tests and observation of the data made me formulate the following conjecture:
*the iteration at which the average grayscale of the canvas becomes equal to the average grayscale of the input picture is a good time to stop execution*, because error (difference between the canvas and the input) is either flattening or even increasing after that point.

### Note of error computation

The current state of the canvas is taken into account in one of the current two error computation functions, but it doesn't significantly improve the quality of the result to use this error as the methot to select the best line at each iteration (due to lines preffering to be drawn on top of already drawn lines to not make any change, and thus not introducing any error).

## What to do next

### Error calculation

Like what is done in the string art paper, downscaling the resolution of the canvas and what it is compared to before iterating over pixels to compute an error may greatly improve the quality of the meaning of the error, as in allows to use the square of the difference (as it is usually used in science to compute error along multiple data points). It may also be analogous to stepping back (as a human) to see the many strings become something that resembles the input.

### Color (still in grayscale) considerations

- Using transparent lines, that make darker and darked sports as they overlap.
- Black and white lines (with no transparency this time) (this will need some algorithmic tweaking to make the algorithm not endup stuck in idiotic loops of doing nothing / canceling its own progress over and over).
- Make each line be of the color of the average grayscale of the pixels it will overwrite.
- Consider grayscales described by n bits (black and white, 4 shades of gray, etc.).

### Halting heuristic

Maybe a good time to stop execution is when the best line that the algorithm could find introduces an error greater or equal to zero, i.e. drawing the best line is worst than doing nothing.

### Preprocessing

Do preprocessing. Maybe by using photoshop, gimp, some python library, etc. As long as it provides state-of-the-art image processing algorithms (instead of handwritten quick-n-dirty code).

Reading the book will be enlightening about some of these algorithms!

### Find HD input pictures

As the previous TODO list said, and the one before it, etc.

### Running in parallel

Maybe not for next week, but it will eventually be an intresting optimization to bring to this project.
