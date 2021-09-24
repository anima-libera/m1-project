
# Meeting 1 - 24/09/2021

## What has been done

### Setup

A GitHub repo in available, with a custom C Linux build system set up.

[The repo.](https://github.com/anima-libera/m1-project)

### Bitmap image emitter

It is one way to be able to visualize rendered images with very portable code
that doesn't require any dependency.

### Line drawing algorithms

Two "test" line drawing programs,
and one "real" line drawing algorithm implementation (Xiaolin Wu).
Of course the Xiaolin Wu version is way better than the custom ones.

### Reading papers

I read the paper given in the project description,
but notes have not been taken yet.

[The paper.](https://www.dmg.tuwien.ac.at/geom/ig/publications/stringart/stringart.pdf)

## What to do next

### Fix the bitmap emitter (by using the BITMAPINFOHEADER header)

Currentrly the emitter reverses red and blue channels, this should be fixed
by using the BITMAPINFOHEADER bitmap header that asks for channel bitmasks.
Using this header will probably enable the use of transparency by introducing
an alpha channel.

[Wikipedia page.](https://en.wikipedia.org/wiki/BMP_file_format)

### Fix the Xiaolin Wu line drawing algorithm implementation

The end points of a line drawn with the current implementation look off.
This probably comes from a mistake introduced by translating the soure
pseudo-code (from Wikipedia) into C, but it could also come from a mistake in
the Wikipedia page's pseudo-code.

[Wikipedia page's pseudo-code.](https://en.wikipedia.org/wiki/Xiaolin_Wu's_line_algorithm#Algorithm)

### Make a bitmap loader.

String art *is* mimicking an image using strings, so getting images onto the
running program's memory seems like the logical next step.

### Implement middle point line drawing algorithm

Multiple line drawing algorithms may come in handy when we will try to see
the difference in speed of execution and accuracy of the result of different
techniques, including different line drawing techniques.

Moreover Caroline sent her slides about this algorithm among other things.

### Make lines have thickness

Currently the thickness of the lines drawn with the current line drawing
implementations only depends on the pixel buffer resolution and is around
exactly 1 pixel. Making thickness a parameter independant from resolution
will be handy for various reasons, including using bigger resolutions
for more accuracy.

### Take notes on the string art paper

A very intresting paper that I should not forget about!

[The paper.](https://www.dmg.tuwien.ac.at/geom/ig/publications/stringart/stringart.pdf)

### Find a new paper to read, and read it

One per week.
