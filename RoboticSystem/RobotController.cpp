#include "RobotController.h"

RobotController::RobotController(PickerRobot robot, Warehouse &warehouseaddr) : robot(robot)
{
	warehouse = &warehouseaddr;
	currentPoint = Point(0, 0);
}

RobotController::~RobotController()
{
}

RobotController::RobotController(const RobotController& rController) :
	robot(rController.robot),
	warehouse(rController.warehouse),
	mapper(rController.mapper),
	startingPoint(rController.startingPoint),
	unloadingPoint(rController.unloadingPoint),
	sortedOrders(rController.sortedOrders),
	currentPoint(rController.currentPoint)
{

}

void RobotController::calculateOptimalPath(std::vector<Order> orders)
{
}

void RobotController::executeOrders(std::vector<Order> orders)
{
}

bool RobotController::processOrder(Order order)
{
	Point orderPosition = warehouse->getCompartmentPosition(order);
	mapper.setCompartmentPosition(orderPosition);
	//robot.setMapper(mapper);

	mapper.printWarehoueMap();
	robot.moveTo(currentPoint, orderPosition);
	robot.pick();
	robot.validate();
	robot.store();

	currentPoint = mapper.getCurrentPosition();

	robot.moveTo(currentPoint, unloadingPoint);
	robot.unload();

	return true;
}

void RobotController::setStartingPoint(Point startPoint)
{
	if (startPoint.getX() < 1 || startPoint.getX() > warehouse->getCols()) {
		cerr << "X coordinate of starting point is out of range!!! " << endl;
		exit(1);
	}
	else if (startPoint.getY() < 1 || startPoint.getY() > warehouse->getRows()) {
		cerr << "Y coordinate of starting point is out of range!!! " << endl;
		exit(1);
	}
	else {
		this->startingPoint = startPoint;
	}
	
}

Point RobotController::getStartingPoint()
{
	return startingPoint;
}

void RobotController::setUnloadingPoint(Point unloadPoint)
{
	if (unloadPoint.getX() < 1 || unloadPoint.getX() > warehouse->getCols()) {
		cerr << "X coordinate of unloading point is out of range!!! " << endl;
		exit(1);
	}
	else if (unloadPoint.getY() < 1 || unloadPoint.getY() > warehouse->getRows()) {
		cerr << "Y coordinate of unloading point is out of range!!! " << endl;
		exit(1);
	}
	else {
		this->unloadingPoint = unloadPoint;
	}

}

Point RobotController::getUnloadingPoint()
{
	return unloadingPoint;
}

void RobotController::startRobot()
{
	mapper = Mapper(warehouse, startingPoint, unloadingPoint);
	mapper.SetupConsole();

	robot.startSerial();
	robot.setMapper(&mapper);

	currentPoint = startingPoint;

	vector<Order> orderList = warehouse->getOrders();
	for (Order order : orderList) {
		processOrder(order);
	}
	cout << "Completed Warehouse: " << warehouse->getWarehouseID() << endl;
}

string RobotController::getWarehouseID()
{
	return warehouse->getWarehouseID();
}

