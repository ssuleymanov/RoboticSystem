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


Point Warehouse::getCompartmentPosition(const Order& order) const
{
	lock_guard<mutex> guard(wh_mutex);
	return Point((order.compartment - 1) / rows + 1, (order.compartment - 1) % rows + 1);
}


void Warehouse::addOrder(const Order& order)
{
	orders.push_back(order);
}

vector<Order> Warehouse::getOrders() const
{
	lock_guard<mutex> guard(wh_mutex);
	return this->orders;
}