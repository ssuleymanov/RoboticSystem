#pragma once
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>
#include <map>
#include <thread>

#include "Warehouse.h"
#include "Point.h"
#include "PickerRobot.h"
#include "RobotController.h"
#include "Printer.h"

using namespace std;


//! A class to manage the robots. 
/*!
Allows to read the order picking list and create robots for each warehouse. It sends orders to the picking robots
*/

class Manager
{
public:
	//! A constructor.
	Manager();

	//! A destructor.
	~Manager();

	//! A function to create picker robots for each warehouse.
	/*!
	\param[in] fileName path to the of the warehouse configuration file
	*/
	void setup(string fileName);

	//! A function to process an oder picking list.
	/*!
	\param[in] fileName path to the order picking list
	*/
	void execute(string oplFile, string articleFile);

	void manualControl(string fileName);

private:
	Printer* printer;
	vector<RobotController> rControllers;
	list<Warehouse> warehouses;
	void addWarehouse(Warehouse wh);
	void addRobotController(RobotController rController);
	Warehouse& getWarehouse(string WarehouseID);
	RobotController& getRobotController(string WarehouseID);
	vector<Order> readOPL(string oplFile, string articleFile);
};