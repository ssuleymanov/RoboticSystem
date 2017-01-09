#pragma once
#include <iostream>

using namespace std;

//! A class to represent a specific location in a 2D grid. 
/*!
Represents a point in Cartesian coordinate system (X,Y).
*/
class Point
{
public:
	//! A constructor to set up a (x,y) location in 2D grid.
	/*!
	\param[in] X parameter for X Coordinate in 2D grid
	\param[in] Y parameter for Y Coordinate in 2D grid
	*/
	Point(int X = 0, int Y = 0);

	//! A copy constructor.
	/*!
	\param[in] p Point object passed by reference
	*/
	Point(const Point& p);
	~Point();

	//! A function to set to change the value of X coordinate.
	/*!
	\param[in] X a new parameter for X Coordinate in 2D grid
	*/
	void setX(int X) { x = X; }

	//! A function to get the value of X coordinate .
	/*!
	\return returns the value of the X coordinate
	*/
	int getX() const { return x; }

	//! A function to set to change the value of Y coordinate .
	/*!
	\param[in] Y a new parameter for Y Coordinate in 2D grid
	*/
	void setY(int Y) { y = Y; }

	//! A function to get the value of Y coordinate .
	/*!
	\return returns the value of the Y coordinate
	*/
	int getY() const { return y; }

	friend ostream& operator<<(std::ostream& os, const Point& point)
	{
		os << "X:"<< point.getX() << " Y:" << point.getY();
		return os;
	}
private:
	int x;
	int y;
};