#pragma once
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <vector>
#include <mutex>

#include "Point.h"
#include "Order.h"

//! A class to represent a warehouse layout. 
/*!
Allows to create a warehouse layout in 2D grid, set starting and unloading positions of a robot and print the map of the layout with moving robot.
*/

class Warehouse
{
public:
	Warehouse(string warehouseID = "A", int rows = 20, int cols = 5);
	Warehouse(const Warehouse& Warehouse);
	~Warehouse();
	string getWarehouseID() const;
	int getRows() const;
	int getCols() const;
	Point getCompartmentPosition(Order order);
	void addOrder(Order order);
	vector<Order> getOrders();

private:
	string warehouseID;
	int rows;
	int cols;
	vector<Order> orders;
	mutable mutex wh_mutex;
};
