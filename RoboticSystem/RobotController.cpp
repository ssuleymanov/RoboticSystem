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


void RobotController::calculateOptimalRoute()
{
	vector<Order> bestRoute, productList, orderList;
	orderList = warehouse->getOrders();
	int best_total = 9999999;
	for (int i = 0; i < orderList.size(); i++) {
		vector<Order> newRoute;
		int total = nearest_neighbor(orderList[i].productID, &newRoute);
		if (total < best_total) {
			best_total = total;
			bestRoute = newRoute;
		}
	}

	mapper.printLog(LOG_INFO, "Best Route: " + to_string(best_total));
	sortedOrders = bestRoute;



	//vector<Order> 
}

void RobotController::calculateOptimalPath(vector<Order> orders)
{
	
}



int RobotController::nearest_neighbor(string productID,vector<Order>* route)
{
	vector<Order> orders = getProductList(warehouse->getOrders());
	vector<Order> sortedRoute;
	vector<Order>::iterator startIt = find_if(orders.begin(), orders.end(), [&](Order order) {return order.productID == productID; });
	int total_dist = moveDistance(startingPoint, warehouse->getCompartmentPosition(*startIt));
	int basketSize = robot.getBasketSize();
	int basket = 1;

	string name = "Route" + warehouse->getWarehouseID();
	printer->printLog(LOG_ACTIONS,name, "Starting..");

	sortedRoute.push_back(*startIt);
	orders.erase(startIt);
	while (orders.size() > 0) {
		int bestDistance = 9999999;
		vector<Order>::iterator bestOrder;
		Point previousPos = warehouse->getCompartmentPosition(sortedRoute.back());
		if (basket == basketSize) {
			total_dist += moveDistance(previousPos, unloadingPoint);
			printer->printLog(LOG_ACTIONS, name, "Moving to unloading, Distance is: " + to_string(moveDistance(previousPos, unloadingPoint)));
			previousPos = unloadingPoint;
			basket = 0;
		}
		for (std::vector<Order>::iterator it = orders.begin(), end = orders.end(); it != end; it++) {
			int distance = 0;
			distance = moveDistance(previousPos, warehouse->getCompartmentPosition(*it));
			if (distance < bestDistance) {
				bestDistance = distance;
				bestOrder = it;
			}
		}
		++basket;
		sortedRoute.push_back(*bestOrder);
		orders.erase(bestOrder);
		printer->printLog(LOG_ACTIONS, name, "Moving to " + to_string(sortedRoute.back().compartment) + ", Distance is: " + to_string(bestDistance));
		total_dist += bestDistance;
	}
	if (basket != 0) { total_dist += moveDistance(warehouse->getCompartmentPosition(sortedRoute.back()), unloadingPoint); }
	printer->printLog(LOG_ACTIONS, name, "Total is: " + to_string(total_dist));

	vector<Order> temp;
	for (int i = sortedRoute.size()-1; i >= 0; --i) {
		temp.push_back(sortedRoute[i]);
	}
	if (getRouteDistance(temp) < total_dist) {
		sortedRoute = temp;
		total_dist = getRouteDistance(sortedRoute);
	}

	*route = sortedRoute;

	return total_dist;
}

int RobotController::getRouteDistance(vector<Order> route) {
	int total_dist = 0;
	int basketSize = robot.getBasketSize();
	int basket = 0;
	Point previousPos = startingPoint;

	for (auto& product : route) {
		total_dist += moveDistance(previousPos, warehouse->getCompartmentPosition(product));
		previousPos = warehouse->getCompartmentPosition(product);
		++basket;

		if (basket == basketSize) {
			total_dist += moveDistance(previousPos, unloadingPoint);
			previousPos = unloadingPoint;
			basket = 0;
		}
	}
	if (basket > 0) { total_dist += moveDistance(previousPos, unloadingPoint); }
	return total_dist;
}


int RobotController::moveDistance(Point start, Point dest) {

	return abs(start.getX() - dest.getX()) + abs(start.getY() - dest.getY());
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
	calculateOptimalPath(orders);
	mapper.printWarehouseMap();
	vector<Order> productList = sortedOrders;
	//vector<Order> productList = getProductList(orders);
	for (auto &order : productList) {
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


