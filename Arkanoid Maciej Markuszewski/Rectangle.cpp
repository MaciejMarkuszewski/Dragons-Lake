#include "Rectangle.h"

Rectangle::Rectangle() = default;

Rectangle::Rectangle(Point l, Point r)
	: l(l), r(r)
{
}

// Returns true if this Rectangle and secondRectangle overlap
bool Rectangle::doOverlap(Rectangle secondRectangle) {
	// if rectangle has area 0, no overlap
	if (l.x == r.x || l.y == r.y || secondRectangle.l.x == secondRectangle.r.x || secondRectangle.l.y == secondRectangle.r.y) {
		return false;
	}
	// If one rectangle is on left side of other
	if (l.x > secondRectangle.r.x || secondRectangle.l.x > r.x) {
		return false;
	}
	// If one rectangle is above other
	if (r.y > secondRectangle.l.y || secondRectangle.r.y > l.y) {
		return false;
	}
	return true;
}