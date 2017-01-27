#pragma once
#ifndef LOADINGDOCK_H
#define LOADINGDOCK_H

#include <map>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream> 

#include "Order.h"
#include "Printer.h"


class LoadingDock {
public:
	//! A constructor.
	LoadingDock();

	//! A function to get a list of all orders for a truck available at the loading dock.
	/*!
	\param[in] truckNr Number of a truck
	\return Vector of orders
	*/
	std::vector<Order> getOrdersperTruck(int truckNr);

	//! A function to add an Order to the LoadingDock
	/*!
	\param[in] order Order to be added to the LoadingDock
	*/
	void addOrdersforTruck(Order order);

	//! A function to notify the loading dock of incorrect orders.
	/*!
	\param[in] customerID ID of the customer with an invalid order
	*/
	void InvalidOrder(string customerID);

	//! A function that removes invalid orders from the unloading area.
	void RemoveInvalidOrders();

	//! A function to print all orders currently in the loading dock sorted by truck
	/*!
	\param[in] printer Pointer to a Printer
	*/
	void printOrders(Printer* printer);

private:
	std::map <int, std::vector<Order>> unloadedOrdersPerTruck;						// unloaded orders per truck
	std::multimap <std::string, Order> unloadedOrdersPerCustomer;				// unloaded orders per customer
	std::multimap <int, Order> customerOrdersbyPriority;							//
	std::map <string, vector<Order>> IncompleteOrders;
	void sortOrderbyPriority();
};


#endif // !LOADINGDOCK_H
