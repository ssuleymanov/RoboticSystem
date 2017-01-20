#pragma once
#ifndef LOADINGDOCK_H
#define LOADINGDOCK_H

#include <map>
#include <vector>
#include <iostream>

#include "Order.h"
#include "Printer.h"

class LoadingDock {
public:
	LoadingDock();
	std::vector<Order> getOrdersperTruck(int truckNr);
	void addOrderforTruck(Order order);
	void printOrders(Printer* printer);

private:
	std::map <int, std::vector<Order>> unloadedOrdersPerTruck;				// unloaded orders per truck

};


#endif // !LOADINGDOCK_H
