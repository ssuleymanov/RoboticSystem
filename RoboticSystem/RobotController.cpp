#include "RobotController.h"

RobotController::RobotController(PickerRobot robot, Warehouse &warehouseaddr) : robot(robot), mapper(&warehouseaddr)
{
	warehouse = &warehouseaddr;
	currentPoint = new Point(1,1);
	printer = Printer::getInstance();
	currentOrderNumber = 0;
	totalOrderNumber = 0;
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
	mapper(rController.mapper),
	currentOrderNumber(rController.currentOrderNumber),
	totalOrderNumber(rController.totalOrderNumber)
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


void RobotController::startManualRobot(vector<Order> orders)
{
	robot.startSerial();
	getOrder(orders);
}

string RobotController::getWarehouseID()
{
	return warehouse->getWarehouseID();
}

PickerRobot & RobotController::getPickerRobot()
{
	return robot;
}

void RobotController::calculateOptimalPath(std::vector<Order> orders)
{
	multimap<int, Order> tempMap;
	multimap<int, Order>::iterator it;
	int distance;

	for (auto& order : orders) {
		distance = (unloadingPoint.getX() - warehouse->getCompartmentPosition(order).getX())^2 + abs(unloadingPoint.getY() - warehouse->getCompartmentPosition(order).getY())^2; // euclidian distance
		tempMap.insert(std::pair<int, Order>(distance, order));
	}

	for (it = tempMap.begin(); it != tempMap.end(); it++) {
		sortedOrders.push_back(it->second);
	}
}

void RobotController::executeOrders(std::vector<Order> orders)
{
	vector<Order> tempOrders;
	Order tempOrder;
	int i, j;

	for (auto &order : orders) {
		tempOrder = order;
		for (i = 0; i < order.quantity; i++) {
			tempOrder.quantity = 1;
			tempOrders.push_back(tempOrder);
		}
	}
	totalOrderNumber = tempOrders.size();

	calculateOptimalPath(tempOrders);
	sort(tempOrders.begin(), tempOrders.end());				// sorts by compartment number

	for (auto &order : tempOrders) {
		if (robot.getNrItemsInBasket() < robot.getBasketSize()) {
			processOrder(order);
		}
		else {
			robot.moveTo(*currentPoint, unloadingPoint);
			robot.unload();
			processOrder(order);
		}
	}

	if (robot.getNrItemsInBasket() != 0) {
		robot.moveTo(*currentPoint, unloadingPoint);
		robot.unload();
	}

	mapper.printString("Warehouse Done!       ",ACTION_NLINE, ACTION_NCOL);
	mapper.printString("Time = " + to_string(robot.getTime()) + " seconds    ", MOVE_NLINE, MOVE_NCOL);
}

bool RobotController::processOrder(Order order)
{

	mapper.printString("Processing Order: " + to_string(order.orderID), ORDER_NLINE , ORDER_NCOL);
	mapper.resetMap();
	Point orderPosition = warehouse->getCompartmentPosition(order);
	mapper.setCompartmentPosition(orderPosition);
	robot.moveTo(*currentPoint, orderPosition);

	if (robot.validate(order)) {
		robot.pick();
		robot.store(order);

		currentOrderNumber++;
		mapper.printString("Progress: " + to_string(currentOrderNumber * 100 / totalOrderNumber) + " %", PROGRESS_NLINE, PROGRESS_NCOL);
	}
	else {
		cerr << "Invalid Product ID: " << order.productID << endl;
		return false;
	}

	return true;
}

bool RobotController::getOrder(vector<Order> orders)
{
	mapper.printWarehouseMap();
	executeOrders(orders);
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


