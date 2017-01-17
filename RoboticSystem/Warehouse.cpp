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
	lock_guard<mutex> guard(wh_mutex);
	return warehouseID;
}

int Warehouse::getRows() const
{
	lock_guard<mutex> guard(wh_mutex);
	return rows;
}

int Warehouse::getCols() const
{
	lock_guard<mutex> guard(wh_mutex);
	return cols;
}


Point Warehouse::getCompartmentPosition(Order order)
{
	lock_guard<mutex> guard(wh_mutex);
	if (order.compartment == 0) {
		order.compartment = 1;
		cout << "Compartment number is zero: " << warehouseID << endl;
	}
	return Point((order.compartment - 1) / rows + 1, (order.compartment - 1) % rows + 1);
}


void Warehouse::addOrder(Order order)
{
	orders.push_back(order);
}

vector<Order> Warehouse::getOrders()
{
	lock_guard<mutex> guard(wh_mutex);
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
