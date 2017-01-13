#include "PickerRobot.h"

PickerRobot::PickerRobot(int basketSize) : basketSize(basketSize)
{
	itemsInBasket = 0;
	timer = 0;
}

PickerRobot::~PickerRobot()
{
}

PickerRobot::PickerRobot(const PickerRobot & robot) :
basketSize(robot.basketSize),
portNumber(robot.portNumber),
baudRate(robot.baudRate),
serial(robot.serial),
mapper(robot.mapper),
itemsInBasket(robot.itemsInBasket),
timer(robot.timer)
{
}

void PickerRobot::moveTo(Point start, Point dest)
{
	int x = 0, y = 0;
	x = dest.getX() - start.getX();
	y = dest.getY() - start.getY();

	string message = "Moving from " + to_string(start.getX()) + "," + to_string(start.getY());
	//mapper->printString(message, ACTION_NLINE, ACTION_NCOL);
	message += " to " + to_string(dest.getX()) + "," + to_string(dest.getY()) + "  ";
	mapper->printString(message, ACTION_NLINE, ACTION_NCOL);

	for (int i = 0; i < abs(x); i++) {
		//cout << "Moving to => " << dest << endl;
		if (x > 0)
			sendCommand(RIGHT);
		else
			sendCommand(LEFT);

		mapper->printWarehouseMap();
	}
	for (int i = 0; i < abs(y); i++) {
		//cout << "Moving to => " << dest << endl;
		if (y > 0)
			sendCommand(UP);
		else
			sendCommand(DOWN);

		mapper->printWarehouseMap();
		
	}
	
	//clog << "\tMoving from " << start.getX() << "," << start.getY();
	//clog << " to " << dest.getX() << "," << dest.getY() << endl;
}


void PickerRobot::setSerialParameters(int portNr, int baud)
{
	portNumber = portNr;
	baudRate = baud;
}

void PickerRobot::setMapper(Mapper *map)
{
	mapper = map;
}

void PickerRobot::startSerial()
{
	if (serial.Open(portNumber, baudRate)) {
		clog << "Port opened succesfully.." << endl;
	}
	else {
		clog << "Failed to open port..!" << endl;
	}
}

void PickerRobot::pick()
{
	mapper->printString("Picking up an item       ",ACTION_NLINE, ACTION_NCOL);
	mapper->printString("                         ", MOVE_NLINE, MOVE_NCOL);
	//cout << "Picking up an item" << endl;
	sendCommand(PICK);
	mapper->printWarehouseMap();
}

bool PickerRobot::validate(Order order)
{
	mapper->printString("Validating an item        ", ACTION_NLINE, ACTION_NCOL);
	mapper->printString("                         ", MOVE_NLINE, MOVE_NCOL);
	//cout << "Validating an item" << endl;
	sendCommand(VALIDATE);

	string productID;
	std::ifstream file("validate.txt");
	while (std::getline(file, productID)) {
		if (order.productID == productID) {
			mapper->printLog("Fetched product ID: " + productID);
			mapper->printLog("Actual product ID: "  + order.productID);
			//cout << "Fetched product ID: " << productID << endl;
			//cout << "Actual product ID: " << order.productID << endl;
			mapper->printWarehouseMap();
			return true;
		}
	}

	mapper->printString("Wrong product ID:        ", ACTION_NLINE, ACTION_NCOL);
	mapper->printString("                         ", MOVE_NLINE, MOVE_NCOL);
	//cout << "Wrong product ID: " << order.productID << endl;
	mapper->printWarehouseMap();
	return false;
}

bool OrderExist(Order order) {
	return true;
}

void PickerRobot::store(Order order)
{
	mapper->printString("Storing an item         ", ACTION_NLINE, ACTION_NCOL);
	mapper->printString("                         ", MOVE_NLINE, MOVE_NCOL);
	//cout << "Storing an item" << endl;
	sendCommand(STORE);
	bool newOrder = true;
	for (auto& bOrder : ordersInBasket) {
		if (bOrder.orderID == order.orderID) {
			bOrder.quantity++;
			newOrder = false;
		}
	}
	if (newOrder) {
		//Order ord = order;
		order.quantity = 1;
		ordersInBasket.push_back(order);
	}
	//ordersInBasket.push_back(order);		// add item to the basket
	mapper->printWarehouseMap();
	itemsInBasket++;
}

void PickerRobot::unload()
{
	mapper->printString("Unloading items         ", ACTION_NLINE, ACTION_NCOL);
	Order tempOrder;
	for (vector<Order>::iterator it = ordersInBasket.begin(); it != ordersInBasket.end(); ++it) {
		for (int i = 0; i < it->quantity; i++) {
			sendCommand(UNLOAD);
			itemsInBasket--;
			mapper->printWarehouseMap();
			//TODO add time for each unload
		}
		mapper->getWarehouse()->getUnloadedOrders().push_back(*it);	
	}
	ordersInBasket.clear();

	itemsInBasket = 0;
}

int PickerRobot::getBasketSize()
{
	return basketSize;
}

int PickerRobot::getNrItemsInBasket()
{
	return itemsInBasket;
}

int PickerRobot::getTime()
{
	return timer;
}

void PickerRobot::sendCommand(const char c)
{
	if (serial.IsOpened()) {
		char message[5] = "F";
		//cout << "Send command => " << c << endl;
		//cout << "Number of items in the basket: " << itemsInBasket << endl;
		mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize) + "    ", BASKET_NLINE, BASKET_NCOL);

		assert(serial.SendData(c));
		mapper->updateWarehouseMap(c);

		if (serial.ReadDataWaiting() > 0) {
			serial.ReadData(message, 1);
			cerr << "ERROR ON: " << message << endl;
		}
	}
	else {
		mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize) + "    ", BASKET_NLINE, BASKET_NCOL);
		mapper->updateWarehouseMap(c);
	}
	timer += MOVE_TIME;
}