#include "LoadingDock.h"

LoadingDock::LoadingDock()
{
}

std::vector<Order> LoadingDock::getOrdersperTruck(int truckNr)
{
	return unloadedOrdersPerTruck[truckNr];
}

void LoadingDock::addOrdersforTruck()
{
	std::multimap <int, Order>::iterator it;

	bool firstOrder = true;
	bool newOrder = true;
	//std::vector<Order> orders = unloadedOrdersPerTruck[order.truckNr];


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

void LoadingDock::sortOrderbyPriority(Order order)
{
	customerOrdersbyPriority.insert(std::pair<int, Order>(order.orderID, order));
}

void LoadingDock::printOrders() {
	std::ofstream outputFile;
	outputFile.open("truckOrders.txt", std::fstream::out);
	if (outputFile.is_open()) {
		for (int i = 1; i < unloadedOrdersPerTruck.size() + 1; i++) {
			outputFile << "Truck: " << i << "\n";
			for (auto& order : unloadedOrdersPerTruck[i]) {
				outputFile << "\tOrder: " << order.orderID << " ProductID: " << order.productID << " Quantity: " << order.quantity << "\n";
			}
			outputFile << "\n";
		}
	}
	else {
		std::cout << "the file cannot be opened \n";
	}

}