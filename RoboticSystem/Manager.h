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
#include <mutex>
#include <rapidxml.hpp>
#include <rapidxml_utils.hpp>

#include "Warehouse.h"
#include "Point.h"
#include "PickerRobot.h"
#include "RobotController.h"
#include "Printer.h"
#include "CollectorRobot.h"

#define COLLECTOR_CONFIG_SIZE 3
#define WAREHOUSE_CONFIG_SIZE 10

#define COLLECTOR_CONFIG_FILE "collector_config.txt"

using namespace std;
using namespace rapidxml;

//! A class to manage the robots and whole logistic process. 
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

	//! A function to Initialize the manager.
	/*!
	\param[in] fileName path to an article list
	*/
	void setup(string config_file, string article_file);

	//! A function to initialize the display.
	

	//! A function to process an oder picking list.
	/*!
	\param[in] fileName path to the order picking list
	*/
	void execute(string oplFile);

	//! A function to read and save an article list.
	/*!
	\param[in] fileName path to the order picking list
	*/
	void readArticles(string articleFile);

	//! A function to process a single order
	/*!
	\param[in] productID ProductID of the product to be picked
	\param[in] quantity Number of products to be picked
	\return returns false if productID could not be found in the article list, else returns true
	*/
	bool manualControl(string productID, int quantity);

	//! A function to check if a product exists in the article list
	/*!
	\param[in] productID ProductID
	\return returns false if productID could not be found in the article list, else returns true
	*/
	bool productValid(string productID);

	//! A function to notify the CollectorRobot when an order has been picked
	/*!
	\param[in] orders Vector of orders that are ready to be collected
	*/
	void orderIsDone(vector<Order> orders);

	//! A function to notify that an order was invalid
	/*!
	\param[in] customerID ID of the customer with an invalid order
	*/
	void orderIsInvalid(string customerID);

	void setupDisplay();

private:
	Printer* printer;
	vector<RobotController> rControllers;
	list<Warehouse> warehouses;
	map<string, Article> articles;
	CollectorRobot collector;
	LoadingDock loadingDock;
	mutex collect_mutex;
	bool menuOn;

	void addWarehouse(Warehouse wh);
	void addRobotController(RobotController rController);
	void ControlPanel(int offset);
	Warehouse& getWarehouse(string WarehouseID);
	RobotController& getRobotController(string WarehouseID);
	vector<Order> readOPL(string oplFile);

};