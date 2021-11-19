
# Meeting 6 - 12/11/2021

## What has been done

### HD canvas to draw thinner lines

Now there is two canvas: the SD one (same resolution than the target canvas and the input picture) and the HD one (on which strings are drawn). It allows to do what was explained in the previous report.

### More data collected for analysis

New error calculations using squares of differences, and other metrics.

## What to do next

### Fix a memory leak

A memory leak made me OOM last time I executed the program.

### Order of drawing

When drawing lines colored with the average color of the covered pixels in the input picture, these lines should be drawn from brightest to darkest.

### Consider all the possible strings

Add the possibility for the string art function to consider and evaluate all the possible strings at each iteration (should be extremely slow, but shoul also produce better output).

### Make random line pools not so random

Try to generate line pools that tend to pass through darker areas (in the target canvas).

### Consider winning lines neighbours

Lines next to winning lines might very well be winning too, maybe check for them.

### Tree-like execution history

Execute the algorithm as usual, but save the entire state at times, and get back from there occasionnally. These backups are like nodes in a tree-like representation of the execution, the results are the leafs. Such execution may be able to cancel unfortunate random choices by going "back in time" and trying something else.

### More work on halting heuristics

The string art representation may need to have an average grayscale darker than the one of the input picture.
