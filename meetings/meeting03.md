
# Meeting 3 - 08/10/2021

## What has been done

### Added pins

The notion of pin set was added to the code base via trigonometry and
iterator-like construct.

### Added mid-point algorithm

As the Xiaolin Wu's implementation, this one too takes a plotter and a pointer
to enable to analyse its output without having to scan the whole canvas.

### Made basic test string art algorithm implementation

And it works! But more work is needed to turn it into a "serious" algorithm.
For now it draws the "best" line per pin (debugging of the algorithm was done
suring the meeting).

### Extracted pictures from the strong art paper

These might come in handy when we will have a competitive algorithm,
in order to compare our results with other algorithms on the same
image benchmark set.

[The string art paper.](https://www.dmg.tuwien.ac.at/geom/ig/publications/stringart/stringart.pdf)

### Read the "picture to lines" paper

I read it, but I need more time and efforts to actually understant every
intresting point made in this paper.

[The picture to lines paper.](http://www.cs.umsl.edu/~kang/Papers/kang_npar07_hi.pdf)

## What to do next

### Improve the current string art algorithm

This can be done by

- drawing multiple lines for each pin (using less pins for performance maybe)
- removing the lines being drawn from the target canvas
- better error calculation ?

### Random pin subset

Iterating over a random subset of pins in a random order will probably be
useful for some algorithms that are to be tested.

### Picture loader

It was already on an earlier TODO list, and it isn't implemented yet.
But now that string art is here, not being able to load "real" pictures
of things to draw with strings will be more and more frustrating.

### (Start to) read an image filtering book

The "picture to lines" paper may be more easy to understand after some
introduction to the field of image filtering and the many algorithms that
are common to it.

Moreover Pr. Caroline Larboulette will send a reference to such a book.

[The picture to lines paper.](http://www.cs.umsl.edu/~kang/Papers/kang_npar07_hi.pdf)
