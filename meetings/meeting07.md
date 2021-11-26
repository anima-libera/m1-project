
# Meeting 7 - 12/11/2021

## What has been done

### Storing results in separate directories

This took way longer that it should have, but it is a step towards automation of testing of different settings and input in one long run.

### Work in progress to fix the mapping between the HD and SD canvas

There seem to be a problem in the mapping of a thin line (drawn on the HD canvas) to the SD canvas, as black lines where not black, and even a (now) black line with a 1:1 scaling ends up non-black in the SD canvas, which is a problem.

### Thoughts about code refactoring

The code base is more and more messy and hard to work with, as too many shortcuts have been taken to get the job done at the cost of slightly less managable code. A big code refactoring is needed, and it should take into account the future addition of support for multi-threading and GPU usage. And I thought about it (but it needs moke thinking at the moment).

### Very quick look at the chapters of the book

I think I should start by Chapter 3 (all of it) for grayscale image preprocessing. Then Chapter 11, as it may help to understand the "pic to lines" paper (that I think may help in intresting preprocessing and maybe even in new string art algorithms). Then chapter 6 for color handling. Then everything else.

## What to do next

### Refactor the code

Making it more managable, thread-safe (for what shoud be thread-safe), etc. Add debugging functions and features to speed up the adding of features (which is slower and slower). Add logging features for almost everything that can be logged (as a debugging help). Also make communication between the C code and the Python code easier.

Concretely, all coordinate systems and color representations will be abstracted away in structs that allow for conversion from and to a standard format and some "local" ones (for example a color may be stored as a `struct {float r, g, b;}` and will support conversions to and from `struct {uint8_t r, g, b;}`, working with these two ways of represeting color had me run into a somehow difficult to find bug in an other project, and this is exactly the king of thing that slow down development here). Having the coordinates of the HD canvas and the SD canvas and all of these depending on their sizes was also a big problem when adding the HD canvas, a `struct {float x, y;}` with `x` and `y` between `-1.0f` and `1.0f` seem like a much better idea (this range is inspired from the default range of axis coordinates in OpenGL). Or having a single resolution (the highest) shoud work too.

Starting from there, every data structure and pattern is to be abstracted away (such as the buffers for data recording during the many iterations of the string art algorithm) so that working on them is easier in the future. A clear graph of "what belongs to what" and "what does what" in treams of memory is also to be drawn (using drawing equipment to make it really easy to navigate) as confusions about this made development harder at times (and also what makes C fun, at the cost of development speed).

Then, refactor the string art algorithm in a way that allow to describe the algorithm that is applied at an iteration in a very customizable way (it is already the case, but not enough!).

Note: It is not about making easy for a user to configure the program (if this ends up true, it would be by accident), it is about making easy for me to add features to the program without spending hours for trivial tasks.

### Go back in the TODO lists

Some items in TODO lists of past meetings haven't been done yet.

### Optimize!

Testing settings, input, code, etc. currently takes ages, it slows down the flow of adding/modifying/fine-tuning features.

### Don't implement preprocessing

It would take too long.

### More lines in one pool can win

The second and third lines of a line pool might be almost as good as the winning line, there is no reason to think they are not. Thus it might speed up the stringartification process by a lot if we keep (possibly) more than one line per iteration.

### Importance layer

Add support for a layer of value (one float per pixel) that represents how much we care about each pixel (as it is done in the string art paper) (each value is between 0 and 1).

### Clamp gray pixels in some interval in the middle and exagerate the pixels we care about

As a preprocessing step, the importance layer may be "consumed" and result in the pixels we care about ending either really dark or really bright, and the pixels we don't care about ending gray.

### Search papers in non-photorealistic rendering

- [SIGGRAF](https://s2021.siggraph.org/full-program/?filter1=sstype123), here portrait processing seem relevant!
- [EXPRESSIVE 2019](http://expressive.graphics/2019/)
- [EUROGRAPHICS](https://eg2022.univ-reims.fr/)

### Read a good part of the chapter 3 of the book

Image preprocessing is important enough to requier state-of-the art techniques, maybe via external professional tools. However, using Photoshop scripting requiers Windows (which I cannot use at the moment) and using GIMP scripting seem hard (or maybe I haven't found a nice tutorial on this aspect of GIMP, provided it even exists). Thus maybe integrating the techniques of image enhancement described in Chapter 3 will be a fine first step towards image preprocessing (moreover some of these techniques don't look too hard to implement (for what I have seen, I may be wrong)).
