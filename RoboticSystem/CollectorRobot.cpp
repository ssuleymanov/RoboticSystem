#include "CollectorRobot.h"

CollectorRobot::CollectorRobot()
{
}

CollectorRobot::CollectorRobot(int basketsize, LoadingDock& ld, string filename)
{
	this->basketSize = basketsize;
	this->loadingDock = &ld;
	this->currentPoint = "LD";
	ready = false;
	nrItemsInBasket = 0;
	totalTime = 0;
	wh_ready = false;

	ifstream file(filename);
	string path;
	int time;
	string value;

	getline(file, value); //Skip Header
	while (getline(file, value)) {
		stringstream ss(value);
		ss >> path >> time;
		path_times[path] = time;
	}
}

void CollectorRobot::setupSerial(int baudrate, int portnumber)
{
	this->baudRate = baudrate;
	this->portNumber = portnumber;
}

void CollectorRobot::startRobot(Printer* printr)
{
	printer = printr;
	if (serial.Open(portNumber, baudRate)) {
		printer->printLog(LOG_INFO,"X", "Port " + to_string(portNumber) + " opened succesfully..");
	}
	else {
		printer->printLog(LOG_ERROR, "X", "Failed to open port " + to_string(portNumber) + " ..!");
	}

	while (!ready) {
		if (wh_ready == true) {
			collectOrder();
			wh_ready = false;
		}
	}
}

void CollectorRobot::addOrder(Order order)
{
	ordersReady.push_back(order);
}

int CollectorRobot::moveTo(string dest)
{
	currentPoint = dest;
	int time = path_times[currentPoint + "to" + dest];
	return time;

}

void CollectorRobot::collectOrder()
{
	if (ordersReady.size() > 0) {
		for (auto &order : ordersReady) {
			if (currentPoint != order.warehouseID) {
				moveTo(order.warehouseID);
			}
			if (nrItemsInBasket < basketSize) {
				ordersInBasket.push_back(order);
				nrItemsInBasket++;
			}
			else {
				unload();
			}
		}
	}
	ordersReady.clear();
}

bool CollectorRobot::sendCommand(const char c)
{

	return true;
}

//void CollectorRobot::loadOrders(Warehouse& warehouse)
//{
//	const int pick_time = 2;
//	string warehouseID = warehouse.getWarehouseID();
//	int movingTime = moveTo(warehouseID);
//	int loadingTime = 0;
//
//	vector<Order> temp;
//
//	for (vector<Order>::iterator it = warehouse.getUnloadedOrders().begin(); it != warehouse.getUnloadedOrders().end(); it++) {
//		if (nrItemsInBasket < basketSize) {
//			ordersInBasket.push_back(*it);
//			loadingTime = loadingTime + pick_time;
//			if (warehouseID == "A") {
//				nrItemsInBasket++;
//			}
//			else if (warehouseID == "B") {
//				nrItemsInBasket = nrItemsInBasket + 2;
//			}
//			else if (warehouseID == "C") {
//				nrItemsInBasket = nrItemsInBasket + 4;
//			}
//		}
//		else {
//			break;
//		}
//	}
//
//	bool inTheBasket = false;
//	
//	// checks if the order is already in the basket of the collector robot, if not it adds it to the temporary vector and updates the warehouse with it later
//	for (vector<Order>::iterator itw = warehouse.getUnloadedOrders().begin(); itw != warehouse.getUnloadedOrders().end(); itw++) {
//		for (vector<Order>::iterator it = ordersInBasket.begin(); it != ordersInBasket.end(); it++) {
//			if (it->productID == itw->productID) {		
//				inTheBasket = true;
//			}
//		}
//		if (inTheBasket == false) {
//			temp.push_back(*itw);
//		}
//		inTheBasket = false;
//	}
//
//	warehouse.updateUnloadedOrders(temp);
//	totalTime = totalTime + movingTime + loadingTime;
//}

int CollectorRobot::unload()
{
	moveTo("LD");		// move to loading dock
	while (nrItemsInBasket > 0) {
		Order order = ordersInBasket.back();
		loadingDock->sortOrderbyPriority(order);
		nrItemsInBasket --;
		ordersInBasket.pop_back();
	}

	return 0;
}

int CollectorRobot::getNrItemsInBasket()
{
	return nrItemsInBasket;
}

void CollectorRobot::isReady()
{
	ready = true;
}

string CollectorRobot::getCurrentPoint()
{
	return currentPoint;
}

void CollectorRobot::warehouseReady()
{
	wh_ready = true;
}

