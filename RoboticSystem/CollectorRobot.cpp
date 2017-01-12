#include "CollectorRobot.h"

CollectorRobot::CollectorRobot(int basketsize, LoadingDock& ld, string filename)
{
	this->basketSize = basketsize;
	this->loadingDock = &ld;
	this->fileName = filename;
	this->currentPoint = "LD";
}

void CollectorRobot::setupSerial(int baudrate, int portnumber)
{
	this->baudRate = baudrate;
	this->portNumber = portnumber;
}

void CollectorRobot::startRobot()
{
	if (serial.Open(portNumber, baudRate)) {
		cout << "Port opened succesfully.." << endl;
	}
	else {
		cout << "Failed to open port..!" << portNumber << endl;
	}
}

int CollectorRobot::moveTo(string dest)
{
	ifstream file(fileName);
	map<string, int> path_times;
	string path;
	int time, temp;
	string value;

	getline(file, value); //Skip Header
	while (getline(file, value)) {
		stringstream ss(value);
		ss >> path >> time;
		path_times[path] = time;
	}

	temp = path_times.find(currentPoint + "to" + dest)->second;
	currentPoint = dest;
	return temp;

}

void CollectorRobot::loadOrders(Warehouse& warehouse)
{
	const int pick_time = 2;
	string warehouseID = warehouse.getWarehouseID();
	int movingTime = moveTo(warehouseID);
	int loadingTime = 0;

	vector<Order> temp;

	for (vector<Order>::iterator it = warehouse.getUnloadedOrders().begin(); it != warehouse.getUnloadedOrders().end(); it++) {
		if (nrItemsInBasket < basketSize) {
			ordersInBasket.push_back(*it);
			loadingTime = loadingTime + pick_time;
			if (warehouseID == "A") {
				nrItemsInBasket++;
			}
			else if (warehouseID == "B") {
				nrItemsInBasket = nrItemsInBasket + 2;
			}
			else if (warehouseID == "C") {
				nrItemsInBasket = nrItemsInBasket + 4;
			}
		}
		else {
			break;
		}
	}

	bool inTheBasket = false;
	
	// checks if the order is already in the basket of the collector robot, if not it adds it to the temporary vector and updates the warehouse with it later
	for (vector<Order>::iterator itw = warehouse.getUnloadedOrders().begin(); itw != warehouse.getUnloadedOrders().end(); itw++) {
		for (vector<Order>::iterator it = ordersInBasket.begin(); it != ordersInBasket.end(); it++) {
			if (it->productID == itw->productID) {		
				inTheBasket = true;
			}
		}
		if (inTheBasket == false) {
			temp.push_back(*itw);
		}
		inTheBasket = false;
	}

	warehouse.updateUnloadedOrders(temp);
	totalTime = totalTime + movingTime + loadingTime;
}

int CollectorRobot::unload()
{
	moveTo("LD");
	for (vector<Order>::iterator it = ordersInBasket.begin(); it != ordersInBasket.end(); it++) {
		loadingDock->addOrderforTruck(it->truckNr,*it);
	}
	ordersInBasket.clear();

	return 0;
}

int CollectorRobot::getNrItemsInBasket()
{
	return nrItemsInBasket;
}

string CollectorRobot::getCurrentPoint()
{
	return currentPoint;
}

