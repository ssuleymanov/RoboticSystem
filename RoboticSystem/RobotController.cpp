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
	currentPoint(rController.currentPoint),
	printer(rController.printer),
	mapper(rController.mapper),
	currentOrderNumber(rController.currentOrderNumber),
	totalOrderNumber(rController.totalOrderNumber)
{
	//if (&rController.mapper == NULL) {
	//	mapper = Mapper();
	//}
	//else {
	//	mapper = rController.mapper;
	//}
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
	int i, j;

	for (auto &order : orders) {
		for (i = 0; i < order.quantity; i++) {
			tempOrders.push_back(order);
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

	//mapper.printWarehouseMap();
	//for(auto &order : orders){
	//	if (order.productID == "DNS-327L") {
	//		cout << "Pause " << endl;
	//		robot.pauseRobot(true);
	//	}
	//	processOrder(order);
	//	if (robot.getNrItemsInBasket() == robot.getBasketSize()) {
	//		robot.moveTo(*currentPoint, unloadingPoint);
	//		robot.unload();
	//	}
	//}
	//if (robot.getNrItemsInBasket() != 0) {
	//	robot.moveTo(*currentPoint, unloadingPoint);
	//	robot.unload();
	//}
	mapper.printString("Warehouse Done!       ",ACTION_NLINE, ACTION_NCOL);
	mapper.printString("Time = " + to_string(robot.getTime()) + " seconds    ", MOVE_NLINE, MOVE_NCOL);
}

bool RobotController::processOrder(Order order)
{
	//if (order.productID == "DNS-327L") {
	//	cout << "STOP " << endl;
	//	robot.emergency_stop(true);
	//	Sleep(10000);
	//	robot.emergency_stop(false);
	//}

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
		//TODO logging should be added here
		cerr << "Invalid Product ID: " << order.productID << endl;
		return false;
	}

	//for (int i = 0; i < order.quantity; i++) {			// if the basket if full, first move to the unloading area to unload the items, then return back
	//	if (robot.getNrItemsInBasket() == robot.getBasketSize()) {
	//		mapper.resetMap();							// make P appear on the map if there are still items left that need to be picked
	//		robot.moveTo(*currentPoint, unloadingPoint);
	//		robot.unload();
	//		robot.moveTo(unloadingPoint, orderPosition);
	//	}

	//	if (robot.validate(order)) {
	//		robot.pick();
	//		robot.store(order);

	//		currentOrderNumber++;
	//		mapper.printString("Progress: " + to_string(currentOrderNumber * 100 / totalOrderNumber) + " %", PROGRESS_NLINE, PROGRESS_NCOL);
	//	}
	//	else {
	//		//TODO logging should be added here
	//		cerr << "Invalid Product ID: " << order.productID << endl;
	//		break;
	//	}
	//}

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

void RobotController::startRobot(Printer* print)
{
	printer = print;
	mapper.Initialize(startingPoint, unloadingPoint, print);
	robot.setMapper(&mapper);
	robot.startSerial();
	currentPoint = mapper.getCurrentPosition();
	
	
	executeOrders(warehouse->getOrders());

	//*currentPoint = startingPoint;		// already set in the Mapper constructor
}

bool RobotController::getOrder(Order ordr)
{
	//cout << "Order number: " << ordr.productID << endl;
	mapper.printWarehouseMap();
	processOrder(ordr);
	robot.moveTo(*currentPoint, unloadingPoint);
	robot.unload();

	//cout << "Completed Warehouse: " << warehouse->getWarehouseID() << endl;
	return true;
}

void RobotController::starManualRobot(Printer* print)
{
	printer = print;
	mapper.Initialize(startingPoint, unloadingPoint, print);
	robot.setMapper(&mapper);
	robot.startSerial();
	currentPoint = mapper.getCurrentPosition();
}

string RobotController::getWarehouseID()
{
	return warehouse->getWarehouseID();
}

