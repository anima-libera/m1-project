
# Meeting 2 - 01/10/2021

## What has been done

### Notes have been taken on the string art paper

[The string art paper.](https://www.dmg.tuwien.ac.at/geom/ig/publications/stringart/stringart.pdf)

### The Xiaolin Wu's line algorithm implementation has been fixed

A for loop has been mistakenly translated from pseudo-code to C (what was
meant to be a `<=` was typed as a `<`, making the line body incomplete).

### The bitmap emitter has been fixed

Now using bitmasks to specify the pixel format in terms of RGB channels,
the emitted bitmaps no longer have the red and blue channels swapped.

### The line drawing function has been made "more general"

It now takes a plotter function pointer and a pointer to data meant to be used
by the given plotter, so that the line it draws can be plotted by an arbitrary
function (that can actually draw the pixels on a canvas, but that can also
just store them in order to do some kinds of analytics on the line, some
debugging, etc.).

The plotters suggested above have been implemented too.

### Genetic string art test

Error variation functions to see how adding a given line to a canvas make
the error (regarding a given target image) go up or down have been produced.
Lines are then mutated little by little(each mutation is kept only if the
error is going down).

It doesn't seem to work as well as expected (but it is not a complete failure
either for some reason) but the error variation computation functions may be
used later thus it was probably not for nothing.

## What to do next

### Add the notion of pin

Ultimately strings are to be streached between pins, and not arbitrary points
on the canvas. Pins are to be implemented somehow.

### Implement a simple string art algorithm

```text
The target is the image that is to be replicated.
The canvas is the blank image on which lines (strings) will be drawn.
Loop for some time {
  Let L be a (small) set of lines not drawn yet.
  For each possible line in L, compute how dark that line is in the target.
  Let D be the line that is the darkest in the target.
  Draw D in the canvas.
  Subtract D from the target.
}.
```

### Implement the mid-point algorithm

Multiple line drawing algorithms may come in handy when we will try to see
the difference in speed of execution and accuracy of the result of different
techniques, including different line drawing techniques.

Moreover Pr. Caroline Larboulette sent her slides about this algorithm
among other things.

Note: This was already in the TODO list of last week.

### Collect the pictures used in the string art paper

These pictures are special in that results already exists for these at targets,
it will allow our results to be compared with already existing results.
The pictures will either be found on the Internet or be extracted fom the pdf.

[The string art paper.](https://www.dmg.tuwien.ac.at/geom/ig/publications/stringart/stringart.pdf)

### Read and take notes on a paper

This paper is about a method that allows to turn an image
(such as a photograph) into a convincing line drawing. It may be useful
as a preprocessing step applied to the input of the optimization algorithm
that will generate the string art instructions. Moreover, it seem to
use a kind of vector field that is locally parallel to important lines
of the input, which may be a useful tool for string art itself.

[The picture to lines paper.](http://www.cs.umsl.edu/~kang/Papers/kang_npar07_hi.pdf)
