#include "LoadingDock.h"

LoadingDock::LoadingDock()
{
}

std::vector<Order> LoadingDock::getOrdersperTruck(int truckNr)
{
	return unloadedOrdersPerTruck[truckNr];
}

void LoadingDock::addOrderforTruck(int truckNr, Order order)
{
	unloadedOrdersPerTruck[truckNr].push_back(order);
}
