#pragma once
#ifndef ROBOTCONTROLLER_H
#define ROBOTCONTROLLER_H
#include <vector>

#include "Point.h"
#include "Order.h"
#include "PickerRobot.h"
#include "Warehouse.h"
#include "Mapper.h"

class RobotController {
public:
	RobotController(PickerRobot robot, Warehouse warehouse);
	~RobotController();
	void calculateOptimalPath(std::vector<Order> orders);
	void executeOrders(std::vector<Order> orders);		// execute orders in the warehouse
	bool processOrder(Order order);						// processes single order, the robot moves to the order location, picks and stores it	
	void setStartingPoint(Point startingPoint);
	Point getStartingPoint();
	void setUnloadingPoint(Point unloadingPoint);
	Point getUnloadingPoint();
	void startRobot();
	bool getOrder(Order ordr);							// manual control of robot, gets a specified order and unloads it in the unloading area

private:
	PickerRobot robot;
	Warehouse warehouse;
	Mapper* mapper;
	Point startingPoint;
	Point unloadingPoint;
	std::vector<Order> sortedOrders;
	Point* currentPoint;

};

#endif // !ROBOTCONTROLLER_H
