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
	mapper.printWarehouseMap();
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

void RobotController::calculateOptimalRoute(vector<Order> orders)
{
	vector<Order> bestRoute, orderList;
	int best_total = 9999999;
	for (int i = 0; i < orders.size(); i++) {
		vector<Order> newRoute;
		int total = nearest_neighbor(orders[i].productID, &newRoute);
		if (total < best_total) {
			best_total = total;
			bestRoute = newRoute;
		}
	}

	mapper.printLog(LOG_SCREEN, "Best Route: " + to_string(best_total));
	sortedOrders = bestRoute;
}



int RobotController::nearest_neighbor(string productID,vector<Order>* route)
{
	vector<vector<int>> dist;
	int totalComp, cols, rows, start, unload;
	start = startingPoint.getX() * startingPoint.getY();
	unload = unloadingPoint.getX() * unloadingPoint.getY();
	cols = warehouse->getCols();
	rows = warehouse->getRows();
	totalComp = cols * rows;
	for (int cx = 1; cx < totalComp; ++cx) {
		vector<int> val;
		for (int cy = 1; cy < totalComp; ++cy) {
			int x = (((cx - 1) / rows) + 1) - (((cy - 1) / rows) + 1);
			int y = (((cx - 1) % rows) + 1) - (((cy - 1) % rows) + 1);		
			val.push_back(abs(x) + abs(y));
		}
		dist.push_back(val);
	}

	vector<Order> orders = getProductList(warehouse->getOrders());
	vector<Order> sortedRoute;
	vector<Order>::iterator startIt = find_if(orders.begin(), orders.end(), [&](Order order) {return order.productID == productID; });
	int total_dist = dist[start][startIt->compartment];
	int basketSize = robot.getBasketSize();
	int basket = 1;

	string name = "Route" + warehouse->getWarehouseID();
	//printer->printLog(LOG_ACTIONS,name, "Starting..");

	sortedRoute.push_back(*startIt);
	orders.erase(startIt);

	while (orders.size() > 0) {
		int bestDistance = 9999999;
		vector<Order>::iterator bestOrder;
		//Point previousPos = compartmentPos[sortedRoute.back().compartment];//warehouse->getCompartmentPosition(sortedRoute.back());
		int previousPos = sortedRoute.back().compartment;
		if (basket == basketSize) {
			total_dist += dist[previousPos][unload];
			//printer->printLog(LOG_ACTIONS, name, "Moving to unloading, Distance is: " + to_string(moveDistance(previousPos, unloadingPoint)));
			previousPos = unload;
			basket = 0;
		}
		for (std::vector<Order>::iterator it = orders.begin(), end = orders.end(); it != end; it++) {
			int distance = 0;
			distance = dist[previousPos][it->compartment];
			if (distance == 0) {
				bestDistance = distance;
				bestOrder = it;
				break;
			}
			if (distance < bestDistance) {
				bestDistance = distance;
				bestOrder = it;
			}
		}
		++basket;
		sortedRoute.push_back(*bestOrder);
		orders.erase(bestOrder);
		//printer->printLog(LOG_ACTIONS, name, "Moving to " + to_string(sortedRoute.back().compartment) + ", Distance is: " + to_string(bestDistance));
		total_dist += bestDistance;
	}
	if (basket != 0) { total_dist += dist[sortedRoute.back().compartment][unload]; }
	//printer->printLog(LOG_ACTIONS, name, "Total is: " + to_string(total_dist));

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

	map<int, Point> compartmentPos;
	for (auto& order : warehouse->getOrders()) {
		compartmentPos[order.compartment] = warehouse->getCompartmentPosition(order);
	}

	for (auto& product : route) {
		total_dist += moveDistance(previousPos, compartmentPos[product.compartment]);
		previousPos = compartmentPos[product.compartment];
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
	vector<Order> tempOrders = getProductList(orders);
	
	/*totalOrderNumber = tempOrders.size();

	calculateOptimalPath(tempOrders);
	sort(tempOrders.begin(), tempOrders.end());*/				// sorts by compartment number
	if (orders.size() > 1) {
		calculateOptimalRoute(orders);
		tempOrders = sortedOrders;
	}
	totalOrderNumber = tempOrders.size();

	for (auto &order : sortedOrders) {
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


