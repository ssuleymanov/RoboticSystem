#pragma once

#ifndef COLLECTORROBOT_H
#define COLLECTORROBOT_H

#ifdef WIN32
#include <windows.h>
#else
#include <ctime>
#endif

#include <vector>
#include <string>
#include <fstream>
#include <map>
#include <sstream>
#include <algorithm>
#include <cassert>
#include <mutex>

#include "Order.h"
#include "Warehouse.h"
#include "Serial.h"
#include "Point.h"
#include "LoadingDock.h"
#include "Printer.h"

#define S_TIME 500

class CollectorRobot {
public:
	CollectorRobot();
	~CollectorRobot();
	CollectorRobot(int basketsize, LoadingDock& ld, string filename);
	//CollectorRobot(const CollectorRobot& collector);
	void setupSerial(int baudrate, int portnumber);
	void startRobot(Printer* printer);
	void addOrder(Order order);
	int unload();									// unload items, 
	int getNrItemsInBasket();
	void isReady();
	string getCurrentPoint();
	void warehouseReady();
	void addWarehouseID(string warehouseID);

private:
	int baudRate;
	int portNumber;
	int basketSize;
	int nrItemsInBasket;
	int totalTime;
	int totTime;
	clock_t startTime;
	vector<string> warehouseIDs;
	bool ready;
	bool wh_ready;
	//std::string startingPoint;
	string currentPoint;
	map<string,vector<Order>> ordersReady;
	vector<Order> ordersInBasket;				// orders stored in the basket of the collector robot
	map<string, int> path_times;
	CSerial serial;
	LoadingDock* loadingDock;
	Printer* printer;
	mutex order_mutex;

	int moveTo(string dest);						// returns the time travelled
	void collectOrder(string warehouseID);
	bool sendCommand(const char c);
	void printMap(string dest);
	Order getOrder(string warehouseID);
};


#endif // !COLLECTORROBOT_H



