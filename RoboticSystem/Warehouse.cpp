#include "Warehouse.h"

Warehouse::Warehouse(string warehouseID, int rows, int cols) :
	warehouseID(warehouseID),
	rows(rows),
	cols(cols)
{

}

Warehouse::Warehouse(const Warehouse& Wh) :
	warehouseID(Wh.getWarehouseID()),
	rows(Wh.getRows()),
	cols(Wh.getCols())
{
}

Warehouse::~Warehouse()
{
}

string Warehouse::getWarehouseID() const
{
	return warehouseID;
}

int Warehouse::getRows() const
{
	return rows;
}

int Warehouse::getCols() const
{
	return cols;
}


Point Warehouse::getCompartmentPosition(Order order)
{
	return Point((order.compartment - 1) / rows + 1, (order.compartment - 1) % rows + 1);
}


void Warehouse::addOrder(Order order)
{
	orders.push_back(order);
}

vector<Order> Warehouse::getOrders()
{
	return this->orders;
}

vector<Order>& Warehouse::getUnloadedOrders()
{
	return unloadedOrders;
}

void Warehouse::updateUnloadedOrders(vector<Order> orders)
{
	unloadedOrders = orders;
}

void Warehouse::setUnloadedOrders(vector<Order> orders)
{
	unloadedOrders = orders;
}
