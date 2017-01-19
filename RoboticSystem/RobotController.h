#pragma once
#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H
#include <vector>
#include <string>

#include "Point.h"
#include "Order.h"
#include "PickerRobot.h"
#include "Warehouse.h"
#include "Mapper.h"

class Manager;

class RobotController {
public:
	RobotController(PickerRobot robot, Warehouse &warehouse);
	~RobotController();
	RobotController(const RobotController& rController);
	void Initialize(Point start, Point unload, Printer* print);
	Point getStartingPoint();
	Point getUnloadingPoint();
	void startRobot();					
	void startManualRobot(Order ordr); // manual control of robot, gets a specified order and unloads it in the unloading area
	string getWarehouseID();

private:
	PickerRobot robot;
	Warehouse* warehouse;
	Printer* printer;
	Mapper mapper;
	Point startingPoint;
	Point unloadingPoint;
	std::vector<Order> sortedOrders;
	Point* currentPoint;

	void calculateOptimalPath(std::vector<Order> orders);
	void executeOrders(std::vector<Order> orders);		// execute orders in the warehouse
	bool processOrder(Order order);						// processes single order, the robot moves to the order location, picks and stores it	
	bool getOrder(Order ordr);
	bool inRange(Point p);

};

#endif // !ROBOTCONTROLLER_H
