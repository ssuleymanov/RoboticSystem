#include "Point.h"

Point::Point(int X, int Y) : 
	x(X), 
	y(Y)
{
}

Point::Point(const Point& p) : 
	x(p.getX()), 
	y(p.getY())
{}

Point::~Point()
{
}