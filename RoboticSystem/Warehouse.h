#pragma once
#include <vector>
#include <mutex>

#include "Point.h"
#include "Order.h"

#define DEFAULT_WAREHOUSE "D"

//! A class to represent a warehouse layout. 
/*!
	Allows to create a warehouse layout in 2D grid, set starting and unloading positions of a robot and print the map of the layout with moving robot.
*/

class Warehouse
{
public:
	//! A constructor. By default creates  a warehouse with one compartment
	/*!
	\param[in] warehouseID Identification for this warehouse
	\param[in] rows number of rows for this warehouse
	\param[in] cols number of cols for this warehouse
	*/
	Warehouse(string warehouseID = DEFAULT_WAREHOUSE, int rows = 1, int cols = 1);

	//! A copy constructor.
	Warehouse(const Warehouse& Warehouse);

	//! A deconstructor.
	~Warehouse();

	//! A function to get the warehouseID.
	/*!
	\return WarehouseID
	*/
	string getWarehouseID() const;

	//! A function to get the rows of the warehouse.
	/*!
	\return Number of rows
	*/
	int getRows() const;

	//! A function to get the collumns of the warehouse.
	/*!
	\return Number of collumns
	*/
	int getCols() const;

	//! A function to get the position of a order in this warehouse.
	/*!
	\param[in] order order to get a compartment position from
	\return Compartment position as a Point
	*/
	Point getCompartmentPosition(const Order& order) const;

	//! A function to add an order to the warehouse.
	/*!
	\param[in] order order to add to the warehouse
	*/
	void addOrder(const Order& order);

	//! A function to get all orders for this warehouse.
	/*!
	\return Vector of orders from this warehouse
	*/
	vector<Order> getOrders() const;

private:
	string warehouseID;
	int rows;
	int cols;
	vector<Order> orders;
	mutable mutex wh_mutex;
};
