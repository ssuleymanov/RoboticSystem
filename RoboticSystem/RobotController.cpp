#include "RobotController.h"

RobotController::RobotController(PickerRobot robot, Warehouse &warehouseaddr) : robot(robot), mapper(&warehouseaddr)
{
	warehouse = &warehouseaddr;
	currentPoint = new Point(1,1);
	printer = Printer::getInstance();
}

RobotController::~RobotController()
{
}

RobotController::RobotController(const RobotController& rController) :
	robot(rController.robot),
	warehouse(rController.warehouse),
	startingPoint(rController.startingPoint),
	unloadingPoint(rController.unloadingPoint),
	sortedOrders(rController.sortedOrders),
	printer(rController.printer),
	mapper(rController.mapper)
{
	robot.setMapper(&mapper);						//Give picker robot new mapper
	currentPoint = mapper.getCurrentPosition();		//Get new pointer to currentpoint
}


Point RobotController::getStartingPoint()
{
	return startingPoint;
}


Point RobotController::getUnloadingPoint()
{
	return unloadingPoint;
}

void RobotController::Initialize(Point start, Point unload, Printer* print) {

	if (inRange(start)) {
		startingPoint = start;
	}
	else {
		mapper.printLog(LOG_ERROR,"Starting point out of range.");
		mapper.printLog(LOG_ERROR, "Point can only be between " + to_string(warehouse->getCols()) + " and " + to_string(warehouse->getRows()));
		exit(1);
	}
	if (inRange(unload)) {
		unloadingPoint = unload;
	}
	else {
		mapper.printLog(LOG_ERROR, "Unloading point out of range.");
		mapper.printLog(LOG_ERROR, "Point can only be between " + to_string(warehouse->getCols()) + " and " + to_string(warehouse->getRows()));
		exit(1);
	}

	printer = print;
	mapper.Initialize(startingPoint, unloadingPoint, print);
	robot.setMapper(&mapper);
	currentPoint = mapper.getCurrentPosition();
}

void RobotController::startRobot()
{
	robot.startSerial();
	executeOrders(warehouse->getOrders());
}


void RobotController::startManualRobot(Order ordr)
{
	robot.startSerial();
	getOrder(ordr);
}

string RobotController::getWarehouseID()
{
	return warehouse->getWarehouseID();
}

void RobotController::calculateOptimalPath(std::vector<Order> orders)
{
}

void RobotController::executeOrders(std::vector<Order> orders)
{
	mapper.printWarehouseMap();
	for (auto &order : orders) {
		processOrder(order);
		mapper.printLog(LOG_INFO, "Finished order: " + to_string(order.orderID));
		if (robot.getNrItemsInBasket() == robot.getBasketSize()) { //If basket is full unload
			robot.moveTo(*currentPoint, unloadingPoint);
			robot.unload();
		}
	}
	if (robot.getNrItemsInBasket() != 0) { //Check if there are still items in the basket
		robot.moveTo(*currentPoint, unloadingPoint);
		robot.unload();
	}
	mapper.printLog(LOG_INFO, "Warehouse " + warehouse->getWarehouseID() + " Done!");
	mapper.printString("Warehouse Done!       ", ACTION_NLINE, ACTION_NCOL);
	mapper.printString("Time = " + to_string(robot.getTime()) + " seconds    ", MOVE_NLINE, MOVE_NCOL);
}

bool RobotController::processOrder(Order order)
{
	mapper.resetMap();
	Point orderPosition = warehouse->getCompartmentPosition(order);
	mapper.setCompartmentPosition(orderPosition);
	robot.moveTo(*currentPoint, orderPosition);			//Move to position of product
	for (int i = 0; i < order.quantity; i++) {			// if the basket if full, first move to the unloading area to unload the items, then return back
		if (robot.getNrItemsInBasket() == robot.getBasketSize()) {
			mapper.resetMap();							// make P appear on the map if there are still items left that need to be picked
			robot.moveTo(*currentPoint, unloadingPoint);
			robot.unload();
			robot.moveTo(unloadingPoint, orderPosition);
		}

		if (robot.validate(order)) {
			robot.pick();
			robot.store(order);
		}
		else {
			//TODO logging should be added here
			mapper.printLog(LOG_ERROR, "Invalid Product ID: " + order.productID);
			return false;
		}

	}

	return true;
}

bool RobotController::getOrder(Order ordr)
{
	mapper.printWarehouseMap();
	processOrder(ordr);
	robot.moveTo(*currentPoint, unloadingPoint);
	robot.unload();

	return true;
}

bool RobotController::inRange(Point p)
{
	if (p.getX() < 1 || p.getX() > warehouse->getCols()) {
		return false;
	}
	else if (p.getY() < 1 || p.getY() > warehouse->getRows()) {
		return false;
	}
	else {
		return true;
	}
}


