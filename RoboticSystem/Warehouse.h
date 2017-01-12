#pragma once
#include <iostream>
#include <iomanip>
#include <windows.h>
#include <vector>

#include "Point.h"
//#include "Order.h"

//! A class to represent a warehouse layout. 
/*!
Allows to create a warehouse layout in 2D grid, set starting and unloading positions of a robot and print the map of the layout with moving robot.
*/

typedef struct {
	string	productID;
	string	warehouseID;
	int		compartment;
}Article;

typedef struct {
	int		compartment;
	string	customerID;
	int		orderID;
	int		priority;
	string	productID;
	int		quantity;
	int		truckNr;
	string	warehouseID;
}Order;

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

};
