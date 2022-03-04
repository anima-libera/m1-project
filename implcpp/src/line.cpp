
#include "line.hpp"
#include <cmath>
#include <iostream>

namespace StringArtRennes
{

RawLine::RawLine(GridCoords a, GridCoords b):
	a{a}, b{b}
{
	;
}

RawLine::Iterator::Iterator():
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

template<typename T>
static inline T square(T x)
{
	return x * x;
}

static inline float distance(GridCoords a, GridCoords b)
{
	return std::sqrt(
		square(static_cast<int>(a.x) - static_cast<int>(b.x)) +
		square(static_cast<int>(a.y) - static_cast<int>(b.y)));
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
		int const error2 = this->error * 2;
		if (error2 >= this->dy)
		{
			this->error += this->dy;
			this->head.x += this->sx;
		}
		if (error2 <= this->dx)
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
