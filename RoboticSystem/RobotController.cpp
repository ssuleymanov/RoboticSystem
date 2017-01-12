#include "RobotController.h"

RobotController::RobotController(PickerRobot robot, Warehouse &warehouseaddr) : robot(robot)
{
	warehouse = &warehouseaddr;
	currentPoint = new Point(1,1);
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
	mapper.printWarehouseMap();
	for(auto &order : orders){
		processOrder(order);
		if (robot.getNrItemsInBasket() == robot.getBasketSize()) {
			robot.moveTo(*currentPoint, unloadingPoint);
			robot.unload();
		}
	}
}

bool RobotController::processOrder(Order order)
{
	mapper.resetMap();
	Point orderPosition = warehouse->getCompartmentPosition(order);
	mapper.setCompartmentPosition(orderPosition);
	robot.moveTo(*currentPoint, orderPosition);
	for (int i = 0; i < order.quantity; i++) {		// if the basket if full, first move to the unloading area to unload the items, then return back
		if (robot.getNrItemsInBasket() == robot.getBasketSize()) {
			mapper.resetMap();							// make P appear on the map if there are still items left that need to be picked
			robot.moveTo(*currentPoint, unloadingPoint);
			robot.unload();
			robot.moveTo(unloadingPoint, orderPosition);
		}
		robot.pick();
		robot.validate();
		robot.store();
	}

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
	currentPoint = mapper.getCurrentPosition();
	robot.setMapper(&mapper);
	robot.startSerial();
	executeOrders(warehouse->getOrders());

	//*currentPoint = startingPoint;		// already set in the Mapper constructor
}

bool RobotController::getOrder(Order ordr)
{
	cout << "Order number: " << ordr.productID << endl;
	mapper.printWarehouseMap();
	processOrder(ordr);
	robot.moveTo(*currentPoint, unloadingPoint);
	robot.unload();

	cout << "Completed Warehouse: " << warehouse->getWarehouseID() << endl;
	return true;
}

string RobotController::getWarehouseID()
{
	return warehouse->getWarehouseID();
}

