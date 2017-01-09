#pragma once
#include <list>
#include <iostream>
#include <string>
#include <sstream>
#include <fstream>
#include <vector>
#include <iterator>

#include "Order.h"
#include "Warehouse.h"
#include "Point.h"
#include "PickerRobot.h"

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
	void execute(string fileName);
private:
	list<PickerRobot> PickerRobots;
	list<Warehouse> warehouses;
	void addWarehouse(Warehouse& wh);
	void addPickerRobot(PickerRobot& robot);
	Warehouse& getWarehouse(string WarehouseID);
	PickerRobot& getPickerRobot(string WarehouseID);
	Order readOPL(string fileName);
};