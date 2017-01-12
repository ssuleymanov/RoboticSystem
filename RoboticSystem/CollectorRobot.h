#pragma once

#ifndef COLLECTORROBOT_H
#define COLLECTORROBOT_H

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>

#include "Order.h"
#include "Warehouse.h"
#include "Serial.h"
#include "Point.h"
#include "LoadingDock.h"

class CollectorRobot {
public:
	CollectorRobot(int basketsize, LoadingDock& ld, string filename);
	void setupSerial(int baudrate, int portnumber);
	void startRobot();
	void loadOrders(Warehouse& warehouse);			// returns the time spent on operation
	int unload();									// unload items, 
	int getNrItemsInBasket();
	string getCurrentPoint();

private:
	int baudRate;
	int portNumber;
	int basketSize;
	int nrTrucks;
	int nrItemsInBasket;
	int totalTime;
	std::string startingPoint;
	std::string currentPoint;
	std::vector<Order> ordersInBasket;				// orders stored in the basket of the collector robot
	CSerial serial;
	string fileName;
	LoadingDock* loadingDock;
	int moveTo(string dest);						// returns the time travelled
};


#endif // !COLLECTORROBOT_H



