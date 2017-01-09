#include "Controller.h"

Controller::Controller()
{
	warehouses.empty();
}

Controller::~Controller()
{
}

void Controller::setup(string fileName)
{

	//Read Warehouse settings
	ifstream file(fileName);
	string value;
	getline(file, value); //Skip Header
	while (getline(file, value)) {
		char whID[50];
		int rows, cols, unload_x, unload_y, start_x, start_y, size;
		size = sscanf_s(value.c_str(), "%s\t%d\t%d\t%d\t%d\t%d\t%d", &whID, 50, &rows, &cols, &unload_x, &unload_y, &start_x, &start_y);
		if (size == 7)
		{
			Warehouse Warehouse(whID, rows, cols, Point(unload_x, unload_y), Point(start_x, start_y));
			PickerRobot robot(Warehouse, 10);
			robot.setSerialParameters(3, 9600);
			addPickerRobot(robot);
		}
		else {
			cout << "[Error] expecting 7 arguments in the warehouse configuration, recieved " << size << endl;
		}
	}

}

void Controller::execute(string fileName)
{
	Order Order = this->readOPL(fileName);
	getPickerRobot(Order.getWarehouseID()).startSerial();

	getPickerRobot(Order.getWarehouseID()).createInstructions(Order);
	getPickerRobot(Order.getWarehouseID()).executeInstructions(Order);
}

void Controller::addWarehouse(Warehouse & wh)
{
	warehouses.push_front(wh);
}

void Controller::addPickerRobot(PickerRobot& robot) {
	PickerRobots.push_front(robot);
}

Warehouse & Controller::getWarehouse(string WarehouseID)
{
	for (list<Warehouse>::iterator i = this->warehouses.begin(), end = warehouses.end(); i != end; i++) {
		if (i->getWarehouseID() == WarehouseID) {
			return *i;
		}
	}
	string err_msg;
	stringstream sstm;
	sstm << "Warehouse " << WarehouseID << " does not exist";
	err_msg = sstm.str();
	throw std::runtime_error(err_msg);
}

PickerRobot & Controller::getPickerRobot(string WarehouseID)
{
	for (list<PickerRobot>::iterator i = this->PickerRobots.begin(), end = PickerRobots.end(); i != end; i++) {
		if (i->getWarehouseID() == WarehouseID) {
			return *i;
		}
	}
	string err_msg;
	stringstream sstm;
	sstm << "PickerRobot for Warehouse " << WarehouseID << " does not exist";
	err_msg = sstm.str();
	throw std::runtime_error(err_msg);
}

Order Controller::readOPL(string fileName)
{
	ifstream file(fileName);
	string value;
	getline(file, value); //Skip Header

	getline(file, value);
	Order currentOrder(value, 1);

	return currentOrder;
}
