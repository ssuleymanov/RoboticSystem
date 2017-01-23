#pragma once
#ifndef LOADINGDOCK_H
#define LOADINGDOCK_H

#include <map>
#include <vector>
#include <iostream>
#include <fstream> 

#include "Order.h"


class LoadingDock {
public:
	LoadingDock();
	std::vector<Order> getOrdersperTruck(int truckNr);
	void addOrdersforTruck();
	void sortOrderbyPriority(Order order);
	void printOrders();

private:
	std::map <int, std::vector<Order>> unloadedOrdersPerTruck;						// unloaded orders per truck
	std::multimap <std::string, Order> unloadedOrdersPerCustomer;				// unloaded orders per customer
	std::multimap <int, Order> customerOrdersbyPriority;							//

};


#endif // !LOADINGDOCK_H
