
# Notes on the String Art Paper

Paper name: **String Art: Towards Computational Fabrication of String Images**.

The paper can be obtained
[here](https://www.dmg.tuwien.ac.at/geom/ig/publications/stringart/stringart.pdf).

## Context

It was sumitted to [Eurographics 2018](https://www.eurographics2018.nl/)
by people of the [GCD](https://gcd.tuwien.ac.at/?page_id=36)
and the [VCC](https://cemse.kaust.edu.sa/vcc/about-vcc).

## Formalization of string art

It formalizes the string art problem as an algorithm specification with
mathematical precison (for streched opaque black strings).

## What to remember

The figure 4 on page 5 summarizes a good line rendering "trick": using the
[mid point line drawing algorithm](https://en.wikipedia.org/wiki/Bresenham%27s_line_algorithm)
on a high resolution pixel grid, then lower the resolution to get a grayscale
anti-aliased rendering of the lines with the same resolution than the input
image.

The 4 ways to join two pins by a streched thread (illustrated in figure 6
on page 7) is something to keep in mind if the resolution is high enough to
make the pin radius significant.

A problem induced by the pin size is represented in figure 7 on page 7 and is
also to be kept in mind.
