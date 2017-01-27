#pragma once
#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H
#include <vector>
#include <string>
#include <algorithm>
#include <map>

#include "Point.h"
#include "Order.h"
#include "PickerRobot.h"
#include "Warehouse.h"
#include "Mapper.h"


class Manager;

//! A class to control a PickerRobot. 
/*!
	Allows the processing of an orderlist and calculates the optimal path for picking multiple orders.
*/

class RobotController {
public:
	//! A constructor.
	/*!
	\param[in] robot PickerRobot to be controlled
	\param[in] &warehouse Reference to a Warehouse this Controller is assigned to
	*/
	RobotController(PickerRobot robot, Warehouse &warehouse);

	//! A deconstructor.
	~RobotController();

	//! A copy constructor.
	RobotController(const RobotController& rController);

	//! A function to initialize the RobotController.
	/*!
	\param[in] start Starting position of the robot
	\param[in] unload Unloading position of the robot
	\param[in] print Pointer to the Printer
	*/
	void Initialize(Point start, Point unload, Printer* print);

	//! A function to start the robot in automatic mode.
	void startRobot();	

	//! A function to start the robot in manual mode.
	/*!
	\param[in] orders A vector of orders to be picked
	*/
	void startManualRobot(vector<Order> orders); // manual control of robot, gets a specified order and unloads it in the unloading area

	//! A function to get the warehouseID.
	/*!
	\return WarehouseID
	*/
	string getWarehouseID();

	//! A function to get the PickerRobot.
	/*!
	\return PickerRobot reference
	*/
	PickerRobot& getPickerRobot();

private:
	PickerRobot robot;
	Warehouse* warehouse;
	Printer* printer;
	Mapper mapper;
	Point startingPoint;
	Point unloadingPoint;
	vector<Order> sortedOrders;
	vector<Order> InvalidProductIDs;
	Point* currentPoint;
	int totalOrderNumber;
	int currentOrderNumber;

	void calculateOptimalPath(std::vector<Order> orders);
	vector<Order> getProductList(vector<Order> orders);
	void executeOrders(std::vector<Order> orders);		// execute orders in the warehouse
	bool processOrder(Order order);						// processes single order, the robot moves to the order location, picks and stores it	
	bool inRange(Point p);

};

#endif // !ROBOTCONTROLLER_H
