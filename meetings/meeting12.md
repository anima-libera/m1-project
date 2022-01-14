
# Meeting 12 - 14/01/2022

## What has been done

### Optimization

Each iteration of the main loop of the algorithm, a loop of lines is generated, each line is evaluated and given a score, and the best line of the pool is drawn (the other are discarded).

This have been optimized as follows: now a parameter allow to choose how many lines are to be drawn at each iteration, they are selected as the N best lines of the pool. In now runs much faster (for generating the pools takes time).

### Started paper

Using the template provided by the targetted conference, mostly raising a skeleton of a plan filled with TODOs about what to be explained.

### Realisation on the original paper

The algorithm presented in the original paper in actually "just" a greedy algorithm that supports the removal of lines (no wonder why it takes so long to run then).

## What to do next

### Paper

Raise an exhaustive list of what have been done that is to be written in the paper draft. And add content to the paper (formalizations of the Strong Art problem and the various extentions and solutions we implemented or thought about).

### Results

Bruteforce and explore by gradient descent the coeficient space corresponding to the multiplicative coefficients of the terms of the sum of all the formulas for computing the score of lines (all the previous heuristics). This seems like a good idea to approximate a super-heuristic for some kind of input.

### Features

- Get the halting condition heuristics up and working again (from past the last refactoring).
- Get the graphics about the monitoring of various measures along the iterations up and working again (from past the last refactoring).
- Try adding parallelism to some parts of the code where it *seems* trivial to add.
- Try to consider Bezier and Hermite curves instead of lines (with some help from [here](http://members.chello.at/easyfilter/bresenham.pdf)).
