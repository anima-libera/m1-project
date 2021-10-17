
# Meeting 4 - 15/10/2021

## What has been done

### Random generator

Including a random generator of mine to select random pins when needed.

### String art partial implementation

Taking a random pin, and for every other pin, compute the error that would
be introduced by dawing a line between those pins, then draw the line that
introduces the smallest error, and erase the same line from the target.

### Input image loading pipeline

To work on "real" input images, a way to use an arbirary image file as the
source of the input pixel grid has been added. It uses the image parsing
abilities of the *matplotlib* Python library, and produces a itermediary
representation of the image readable by our C program.

### First results

The features mentionned above were used to produce some promissing results
(even though these are only for testing).

## What to do next

### Going back to grayscale

Color is harder to handle for now, so we are going to only handle grayscale
for now, but without throwing away color channel handling.

### Allow different colors for strings

Allow a set of colors (grayscale for now) to be the colors of lines being drawn
by string art algorithms.

### Find the input pictures of the string art paper in HD

We already have the pictures contained in the pdf, but higher definition
might be needed.

### Truly pick random subset of potential lines to draw

This may be less good than the current algorithm, so it will be done as
an other test.

### Compute the error of a line taking into account the current canvas

For now, computing the error introduced by a potential line, the current
state of the canvas on which the line would be drawn is not read (in a way,
it is assumed to be plain white, which is lass and less true as more lines
are drawn on it). For various reasons, it really might be better to take it
into account.

### Easier image loading

Currently the input image leading pipeline requiers modifying a Python script
and some C code, but it should be way simpler than that.

### Add more string art algorithms, and compare them

There are some implementation available on GitHub repos.

### Try different image preprocessing methods on input images

First by applying trivial C-written algorithms, then try to use scriptable
tools such as Gimp, Photoshop, or some Python lib maybe.

### Halt execution at some non-abitrary number of drawn lines

Currently, the string art algorithm used here takes a number of lines to draw
as an input. However, as a graph obtained during testing shows, the error
introduced by each new line drawn keeps rising in a way that somehow ressemble
square root or logarithm or something similar. Maybe there is a good way to
use this parameter to decide of a good way to know when to stop adding new
lines.

### Start reading the Digital Image Processing book

Start reading it!
