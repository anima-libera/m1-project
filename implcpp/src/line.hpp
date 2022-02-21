
#ifndef HEADER_LINE_
#define HEADER_LINE_

#include "grid.hpp"

namespace StringArtRennes
{

/* Class that can be used in range-based for loops to iterate over all the grid coordinates
 * of a line, generated with the mid-point line drawing algorithm. */
class RawLine
{
public:
	GridCoords a, b; /* Both are included. */

public:
	RawLine(GridCoords a, GridCoords b);

public:
	class Iterator
	{
	public:
		GridCoords head, end;
		int dx, dy, sx, sy, error;
		bool is_past_the_end; /* True iff equal to the past-the-end iterator. */
	public:
		Iterator(); /* Past-the-end. */
		Iterator(GridCoords head, GridCoords end); /* Not past-the-end. */
		bool operator==(Iterator const& right) const;
		bool operator!=(Iterator const& right) const;
		Iterator& operator++();
		Iterator operator++(int);
		GridCoords operator*() const;
	};

public:
	Iterator begin() const;
	Iterator end() const;
};

} /* StringArtRennes */

#endif /* HEADER_LINE_ */
