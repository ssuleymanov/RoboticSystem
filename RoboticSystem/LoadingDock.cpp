#include "LoadingDock.h"

LoadingDock::LoadingDock()
{
}

std::vector<Order> LoadingDock::getOrdersperTruck(int truckNr)
{
	return unloadedOrdersPerTruck[truckNr];
}

void LoadingDock::addOrderforTruck(Order order)
{
	bool newOrder = true;
	std::vector<Order> orders = unloadedOrdersPerTruck[order.truckNr];
	for (std::vector<Order>::iterator it = orders.begin(), end = orders.end(); it != end; it++) {
		if (it->orderID == order.orderID) {
			it->quantity += order.quantity;
			newOrder = false;
		}
	}
	if (newOrder) {
		orders.push_back(order);
		unloadedOrdersPerTruck[order.truckNr].push_back(order);
	}
	else {
		unloadedOrdersPerTruck[order.truckNr] = orders;
	}
}

void LoadingDock::printOrders() {
	for (int i = 1; i < unloadedOrdersPerTruck.size()+1; i++) {
		std::cout << "Truck: " << i << "\n";
		for (auto& order : unloadedOrdersPerTruck[i]) {
			std::cout << "\tOrder: " << order.orderID << " ProductID: " << order.productID << " Quantity: " << order.quantity << "\n";
		}
		std::cout << "\n";
	}
}