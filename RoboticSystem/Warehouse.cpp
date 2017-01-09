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
	return Point((order.compartmentPosition - 1) / rows + 1, (order.compartmentPosition - 1) % rows + 1);
}


void Warehouse::setOrders(vector<Order> orders)
{
	this->orders = orders;
}

vector<Order> Warehouse::getOrders()
{
	return this->orders;
}
