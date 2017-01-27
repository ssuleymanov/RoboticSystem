#pragma once

#ifndef COLLECTORROBOT_H
#define COLLECTORROBOT_H

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

//Commands
#define LOADINGDOCK "LD"
#define MOVE_COMPLETE 'R'

//Icons
#define COLLECTOR		" X "
#define EMPTY			"   "

//Map parts
#define TL_CORNER		string(1, 201)
#define TR_CORNER		string(1, 187)
#define BL_CORNER		string(1, 200)
#define BR_CORNER		string(1, 188) 

#define H_STRAIGHT		string(3, 205)
#define V_STRAIGHT		string(1, 186)

#define T_TCONNECTOR	string(1, 203)
#define B_TCONNECTOR	string(1, 202)
#define L_TCONNECTOR	string(1, 204) 
#define R_TCONNECTOR	string(1, 185)

#define PCONNECTOR		string(1,206)

class CollectorRobot {
public:
	//! A default constructor.
	CollectorRobot();

	//! A deconstructor.
	~CollectorRobot();

	//! A constructor.
	/*!
	\param[in] basketsize Basket size of the robot (amount of products that the robot can carry at once)
	\param[in] ld Reference to a LoadingDock
	\param[in] filename Path to a file of travel times between warehouses
	*/
	CollectorRobot(LoadingDock& ld, string filename);

	//! A function to set the basket size and serial parameters.
	/*!
	\param[in] baudrate baudrate
	\param[in] baudrate baudrate
	\param[in] portnumber port number
	*/
	void InitCollector(int basketsize, int baudrate, int portnumber);

	//! A function to start the CollectorRobot.
	/*!
	\param[in] printer Pointer to the Printer
	*/
	void startRobot(Printer* printer);

	//! A function to add the name of a Warehouse to the CollecctorRobot.
	/*!
	\param[in] warehouseID WarehouseID
	*/
	void addWarehouseID(string warehouseID);

	//! A function to add an order to the queue of the CollectorRobot.
	/*!
	\param[in] order Order to be added to the CollectorRobot
	*/
	void addOrder(Order order);

	//! A function to notify the CollectorRobot that no more orders will be added to the queue
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



