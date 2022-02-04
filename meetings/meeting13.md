
# Meeting 13 - 21/01/2022

## What has been done

### Halting heuristics

They are back! Just like before.

### Coefficients of the all-in-one line scoring heuristic

Possible lines are given a score according to some heuristic to be sorted, the top ones are drawn and the rest are discarded. The way the score is computed is the line scoring heuristic. In order to search a good heuristic, a new heuristic has been implemented, which makes the sum of all the other heuristics, with multiplicative coefficients given via the command line interface.

### Paper

Some paragraphs have been added to the paper, mostly to get feedback on the wording and the releavce of the information given in the text as well as its density and level of language.

## What to do next

### What to search for

What to maximize? Quality (low error) and speed (low time of execution).
There is no sigle best set of such parameters, only tradeoffs.
One set A is really better than an other set B if A is better than B in both quality and speed.
Thus what we should obtain is a "front" of best sets (plotting of sets in quality/speed graph).

### Set of parameters/coefs to optimize on

- score formula heuristic mix coefs ([-1.0, 1.0]^n)
- line pool size ([1, 10000])
- line number chosen per iteration ([1, n/2])
- line opacity ([0.05, 1.0])
- error formula ({diff * diff, abs(diff)})
- input resolution factor ({1, 2, 3, 4})
- pin count ([20, 500])

### How to search for it

- High dimension input space exploration with a partial order on the output.
- Some dimensions are discreet, some are continuous.
- Always start again from random spots occasionnaly to avoid missing too much local maximas.
- Try picking a set of points with a lage convex coverage of the input space.
- Try picking random points between two good points.
- Try picking random points randomly close to a good point, steer toward better results.
- Don't discard points that are not best points but still womehow close to the front.
- Discard points that seem to not steer towards the front despite the any effort.
Maybe have diagonal bands in the output space to have total orders in subsets of inputs, and maybe keep like the 3~4 best inputs in each band (merging inputs that are getting too close, and increasing some counters to remember that these inputs seem to attract a lot of steering points).
- Timeout (so that the whole process isn't made impossible by degenerate inputs).

### In practice

The C program shall take a set of parameters/coefs wia the command line interface, and get the computation done for one point in the input parameter space. Then a Python 3 script is to call the C program numerous times with relevant parameters to get the performance of, and do the whole process talked about above to work out what this "front" is to have the bigger picture of the best sets of parameters for this trade-off.

### Paper

Do the paper. The deadline is just after the next meeting, thime shall not be wasted on matters of irrelevance.
