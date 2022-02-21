
#include "line.hpp"
#include <cmath>

namespace StringArtRennes
{

RawLine::RawLine(GridCoords a, GridCoords b):
	a{a}, b{b}
{
	;
}

RawLine::Iterator::Iterator():
	head{0, 0}, end{0, 0},
	is_past_the_end{true}
{
	;
}

RawLine::Iterator::Iterator(GridCoords head, GridCoords end):
	/* Mid-point line drawing variables initialization. */
	head{head}, end{end},
	dx{std::abs(end.x - head.x)}, dy{-std::abs(end.y - head.y)},
	sx{head.x < end.x ? 1 : -1}, sy{head.y < end.y ? 1 : -1},
	error{this->dx + this->dy},
	is_past_the_end{false}
{
	;
}

bool RawLine::Iterator::operator==(Iterator const& right) const
{
	/* This should only be called by ranged-based for loop internals hopefully...
	 * Wh- what have I done?! */
	return this->is_past_the_end && right.is_past_the_end;
}

bool RawLine::Iterator::operator!=(Iterator const& right) const
{
	return not (*this == right);
}

RawLine::Iterator& RawLine::Iterator::operator++()
{
	/* Mid-point line drawing loop iteration, stepping forward a single pixel. */
	if (this->head == this->end)
	{
		this->is_past_the_end = true;
	}
	else
	{
		if (this->error * 2 >= this->dy)
		{
			this->error += this->dy;
			this->head.x += this->sx;
		}
		if (this->error * 2 <= this->dx)
		{
			this->error += this->dx;
			this->head.y += this->sy;
		}
	}
	return *this;
}

RawLine::Iterator RawLine::Iterator::operator++(int)
{
	Iterator pre_increment_state = *this;
	++(*this);
	return pre_increment_state;
}

GridCoords RawLine::Iterator::operator*() const
{
	return this->head;
}

RawLine::Iterator RawLine::begin() const
{
	return Iterator{this->a, this->b};
}

RawLine::Iterator RawLine::end() const
{
	return Iterator{};
}

} /* StringArtRennes */
