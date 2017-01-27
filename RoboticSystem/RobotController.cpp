#include "RobotController.h"

RobotController::RobotController(PickerRobot robot, Warehouse &warehouseaddr) : robot(robot), mapper(&warehouseaddr)
{
	warehouse = &warehouseaddr;
	currentPoint = new Point(1,1);
	printer = Printer::getInstance();
	currentOrderNumber = 0;
	totalOrderNumber = 1;
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
	mapper.printWarehouseMap();
	printer->printString(warehouse->getWarehouseID(),1,3, "Warehouse " + warehouse->getWarehouseID());
	executeOrders(orders);
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
	//multimap<int, Order> distanceToOrder;					// map of distance to orders from the unloading point
	map<int, multimap<int, Order>> sortedDistanceOrder;		// sorted orders by row and by distance that is close to the unloading point
	map<int, multimap<int, Order>>::iterator iter;
	multimap<int, Order>::iterator it;
	int distance;

	for (auto& order : orders) {
		distance = (unloadingPoint.getX() - warehouse->getCompartmentPosition(order).getX()) + abs(unloadingPoint.getY() - warehouse->getCompartmentPosition(order).getY()); // euclidian distance
		sortedDistanceOrder[warehouse->getCompartmentPosition(order).getY()].insert(std::pair<int, Order>(distance, order));
	}

	for (iter = sortedDistanceOrder.begin(); iter != sortedDistanceOrder.end(); iter++) {
		for (it = iter->second.begin(); it != iter->second.end(); it++) {
			sortedOrders.push_back(it->second);
		}
	}
}

vector<Order> RobotController::getProductList(vector<Order> orders)
{
	vector<Order> products;
	for (auto& order : orders) {
		int quantity = order.quantity;
		order.quantity = 1;
		for (int i = 0; i < quantity; i++) {
			products.push_back(order);
		}
	}
	return products;
}

void RobotController::executeOrders(std::vector<Order> orders)
{
	vector<Order> tempOrders = getProductList(orders);
	
	if (orders.size() > 1) {
		calculateOptimalPath(tempOrders);
		sort(tempOrders.begin(), tempOrders.end());			// sorts by compartment number
	}
	else if (orders.size() == 0) {
		printer->printLog(LOG_ERROR, warehouse->getWarehouseID(), "The OPL does not contain any orders.");
		return;
	}
	else {
		sortedOrders = tempOrders;
	}
	totalOrderNumber = tempOrders.size();
	mapper.printString("Progress: 0 % (0/" + to_string(totalOrderNumber) + ")", PROGRESS_NLINE, PROGRESS_NCOL);
	for (auto &order : sortedOrders) {
		if (robot.getNrItemsInBasket() == robot.getBasketSize()) {
			robot.moveTo(*currentPoint, unloadingPoint);
			robot.unload();
		}
		//processOrder(order);
		if (find_if(InvalidProductIDs.begin(), InvalidProductIDs.end(), [=](Order product) {return(product.productID == order.productID && product.orderID == order.orderID); }) == InvalidProductIDs.end()) {
			if (!processOrder(order)) {
				InvalidProductIDs.push_back(order);
			}
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
		mapper.printString("Progress: " + to_string(currentOrderNumber * 100 / totalOrderNumber) + " % (" + to_string(currentOrderNumber) + "/" + to_string(totalOrderNumber) + ")" , PROGRESS_NLINE, PROGRESS_NCOL);
	}
	else {
		//printer->printLog(LOG_ERROR, warehouse->getWarehouseID(), "Invalid Product ID: " + order.productID);
		return false;
	}

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


