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
				if (order.productID == it->second.productID && order.orderID == it->second.orderID) {
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
	if (IncompleteOrders.find(order.customerID) != IncompleteOrders.end()) {
		bool newOrder = true;
		for (auto& product : IncompleteOrders[order.customerID]) {
			if (product.productID == order.productID && product.orderID == order.orderID) {
				product.quantity++;
				newOrder = false;
			}
		}
		if (newOrder) {
			IncompleteOrders[order.customerID].push_back(order);
		}
	}
	else {
		customerOrdersbyPriority.insert(std::pair<int, Order>(order.orderID, order));
	}
}

void LoadingDock::InvalidOrder(string customerID)
{
	if (IncompleteOrders.find(customerID) == IncompleteOrders.end()) {
		IncompleteOrders[customerID];
	}
}

void LoadingDock::RemoveInvalidOrders()
{
	for (int i = 1; i < unloadedOrdersPerTruck.size() + 1; i++) {
		vector<Order> temp = unloadedOrdersPerTruck[i];
		for (auto order : temp) {
			if (IncompleteOrders.find(order.customerID) != IncompleteOrders.end()) {
				IncompleteOrders[order.customerID].push_back(order);
				unloadedOrdersPerTruck[i].erase(find_if(unloadedOrdersPerTruck[i].begin(), unloadedOrdersPerTruck[i].end(), [=](Order product) {return(product.productID == order.productID && product.orderID == order.orderID); }));
			}
		}
	}
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

	for (auto customer : IncompleteOrders) {
		printer->printLog(LOG_ACTIONS, "IncorrectOrder", "CustomerID: " + customer.first);
		vector<Order> temp = customer.second;
		sort(temp.begin(), temp.end(), [](Order j, Order k) {return (j.priority < k.priority); });
		for (auto& order : temp) {
			printer->printLog(LOG_ACTIONS, "IncorrectOrder", "\tOrder: " + to_string(order.orderID) + " ProductID: " + order.productID + " Quantity: " + to_string(order.quantity));
		}
	}
}