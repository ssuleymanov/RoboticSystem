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
		if (it->orderID == order.orderID && it->productID == order.productID) {
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

void LoadingDock::printOrders(Printer* printer) {
	for (int i = 1; i < unloadedOrdersPerTruck.size()+1; i++) {
		printer->printLog(LOG_ACTIONS, "LoadingDock", "Truck: " + to_string(i));
		vector<Order> temp = unloadedOrdersPerTruck[i];
		sort(temp.begin(), temp.end(), [](Order j, Order k) {return (j.priority < k.priority); });
		for (auto& order : temp) {

			printer->printLog(LOG_ACTIONS, "LoadingDock", "\tOrder: " + to_string(order.orderID) + " ProductID: " + order.productID + " Quantity: " + to_string(order.quantity));
		}
		printer->printLog(LOG_ACTIONS, "LoadingDock", " ");
	}
}