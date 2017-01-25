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
	~CollectorRobot();
	CollectorRobot(int basketsize, LoadingDock& ld, string filename);
	void setupSerial(int baudrate, int portnumber);
	void startRobot(Printer* printer);
	void addWarehouseID(string warehouseID);
	void addOrder(Order order);
	void isReady();

private:
	int baudRate;
	int portNumber;
	int basketSize;
	int nrItemsInBasket;
	int totalTime;
	bool ready;
	string currentPoint;
	mutex order_mutex;
	vector<string> warehouseIDs;
	map<string,vector<Order>> ordersReady;
	vector<Order> ordersInBasket;				// orders stored in the basket of the collector robot
	map<string, int> path_times;
	CSerial serial;
	LoadingDock* loadingDock;
	Printer* printer;

	Order getOrder(string warehouseID);
	void collectOrder(string warehouseID);
	int unload();									// unload items, 
	int moveTo(string dest);						// returns the time travelled
	bool sendCommand(const char c);
	void printMap(string dest);
	
};


#endif // !COLLECTORROBOT_H



