#include "LoadingDock.h"

LoadingDock::LoadingDock()
{
}

std::vector<Order> LoadingDock::getOrdersperTruck(int truckNr)
{
	return unloadedOrdersPerTruck[truckNr];
}

void LoadingDock::sortOrderbyPriority()
{
	std::multimap <int, Order>::iterator it;

	bool firstOrder = true;
	bool newOrder = true;

	for (it = customerOrdersbyPriority.begin(); it != customerOrdersbyPriority.end(); it++) {
		if(firstOrder){
			unloadedOrdersPerTruck[it->second.truckNr].push_back(it->second);
			firstOrder = false;
		}
		else {
			for(auto& order : unloadedOrdersPerTruck[it->second.truckNr]){
				if (order.productID == it->second.productID) {
					order.quantity++;
					newOrder = false;
				}
			}
			if(newOrder){
				unloadedOrdersPerTruck[it->second.truckNr].push_back(it->second);
			}		
		}
		newOrder = true;
	}
}

void LoadingDock::addOrdersforTruck(Order order)
{
	customerOrdersbyPriority.insert(std::pair<int, Order>(order.orderID, order));
}

void LoadingDock::printOrders(Printer* printer) {
	sortOrderbyPriority();
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