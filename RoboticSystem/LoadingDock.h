#pragma once
#ifndef LOADINGDOCK_H
#define LOADINGDOCK_H

#include <map>
#include <vector>

#include "Order.h"

class LoadingDock {
public:
	LoadingDock();
	std::vector<Order> getOrdersperTruck(int truckNr);
	void addOrderforTruck(int truckNr, Order order);

private:
	std::map <int, std::vector<Order>> unloadedOrdersPerTruck;				// unloaded orders per truck

};


#endif // !LOADINGDOCK_H
