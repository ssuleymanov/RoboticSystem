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
	int time = path_times[currentPoint + "to" + dest];
	currentPoint = dest;
	for (int i = 0; i < time; i++) {
		sendCommand(currentPoint.at(0));
	}
	sendCommand('R');
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
	if (serial.IsOpened()) {
		char message[5] = "F";
		assert(serial.SendData(c));

		if (serial.ReadDataWaiting() > 0) {
			serial.ReadData(message, 1);
		}

		Sleep(100);
	}
	else {
		cout << "Serial port is not open!!! \n";
	}
	return true;
}

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

