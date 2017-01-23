#pragma once

#ifndef COLLECTORROBOT_H
#define COLLECTORROBOT_H

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <mutex>

#include "Order.h"
#include "Warehouse.h"
#include "Serial.h"
#include "Point.h"
#include "LoadingDock.h"
#include "Printer.h"

class CollectorRobot {
public:
	CollectorRobot();
	CollectorRobot(int basketsize, LoadingDock& ld, string filename);
	void setupSerial(int baudrate, int portnumber);
	void startRobot(Printer* printer);
	void addOrder(Order order);
	//void loadOrders(Warehouse& warehouse);			// returns the time spent on operation
	int unload();									// unload items, 
	int getNrItemsInBasket();
	void isReady();
	string getCurrentPoint();
	void warehouseReady();

private:
	int baudRate;
	int portNumber;
	int basketSize;
	int nrItemsInBasket;
	int totalTime;
	bool ready;
	bool wh_ready;
	//std::string startingPoint;
	std::string currentPoint;
	std::vector<Order> ordersReady;
	std::vector<Order> ordersInBasket;				// orders stored in the basket of the collector robot
	map<string, int> path_times;
	CSerial serial;
	LoadingDock* loadingDock;
	Printer* printer;
	int moveTo(string dest);						// returns the time travelled
	void collectOrder();
	bool sendCommand(const char c);
};


#endif // !COLLECTORROBOT_H



