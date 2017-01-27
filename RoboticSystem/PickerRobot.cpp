#include "PickerRobot.h"
#include "Manager.h"

PickerRobot::PickerRobot(int basketSize, Manager &manager) : basketSize(basketSize), manager(&manager)
{
	itemsInBasket = 0;
	timer = 0;
	stop = false;
	pause = false;
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
timer(robot.timer),
manager(robot.manager),
stop(robot.stop),
pause(robot.pause)
{
}

void PickerRobot::moveTo(Point start, Point dest)
{
	int x = 0, y = 0;
	x = dest.getX() - start.getX();
	y = dest.getY() - start.getY();

	string message = "Moving from " + to_string(start.getX()) + "," + to_string(start.getY());
	message += " to " + to_string(dest.getX()) + "," + to_string(dest.getY());
	mapper->printString(message, ACTION_NLINE, ACTION_NCOL);
	mapper->printLog(LOG_ACTIONS,message);

	for (int i = 0; i < abs(x); i++) {
		if (x > 0) {
			sendCommand(RIGHT);
		}
		else {
			sendCommand(LEFT);
		}
		mapper->printWarehouseMap();
	}
	for (int i = 0; i < abs(y); i++) {
		if (y > 0) {
			sendCommand(UP);
		}
		else {
			sendCommand(DOWN);
		}
		mapper->printWarehouseMap();
	}
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
		mapper->printLog(LOG_INFO,"Port " + to_string(portNumber) + " opended succesfully..");
		mapper->printLog(LOG_SCREEN, "Port " + to_string(portNumber) + " opended succesfully..");
	}
	else {
		mapper->printLog(LOG_INFO, "Failed to open port " + to_string(portNumber) + "..!");
		mapper->printLog(LOG_ERROR,"Failed to open port " + to_string(portNumber) + "..!");
	}
}

void PickerRobot::pick()
{
	mapper->printString("Picking up an item",ACTION_NLINE, ACTION_NCOL);
	mapper->printString(" ", MOVE_NLINE, MOVE_NCOL);
	mapper->printLog(LOG_ACTIONS,"Picking up an item");
	sendCommand(PICK);
	mapper->printWarehouseMap();
}

bool PickerRobot::validate(Order order)
{
	mapper->printString("Product ID: " + order.productID, MOVE_NLINE, MOVE_NCOL);
	sendCommand(VALIDATE);

	string productID;
	std::ifstream file("validate.txt");
	while (std::getline(file, productID)) {
		if (order.productID == productID) {
			mapper->printLog(LOG_ACTIONS, "Validation OK, product is: " + order.productID);
			mapper->printWarehouseMap();
			return true;
		}
	}
	manager->orderIsInvalid(order.customerID);
	mapper->printLog(LOG_ERROR,"Validation ERROR. Order: " + to_string(order.orderID) + " ProductID: " +  order.productID);
	mapper->printString("Wrong product ID found.", ACTION_NLINE, ACTION_NCOL);
	mapper->printString(" ", MOVE_NLINE, MOVE_NCOL);
	mapper->printWarehouseMap();
	return false;
}

void PickerRobot::store(Order order)
{
	mapper->printString("Storing an item", ACTION_NLINE, ACTION_NCOL);
	mapper->printString(" ", MOVE_NLINE, MOVE_NCOL);
	mapper->printLog(LOG_ACTIONS,"Storing item");
	sendCommand(STORE);
	ordersInBasket.push_back(order);
	mapper->printWarehouseMap();
	itemsInBasket++;

	while (pause == true) {}
}

void PickerRobot::unload()
{
	mapper->printString("Unloading items", ACTION_NLINE, ACTION_NCOL);
	mapper->printString(" ", MOVE_NLINE, MOVE_NCOL);
	mapper->printLog(LOG_ACTIONS,"Unloading items");
	Order tempOrder;
	for (vector<Order>::iterator it = ordersInBasket.begin(); it != ordersInBasket.end(); ++it) {
		itemsInBasket--;
		sendCommand(UNLOAD);
		mapper->printWarehouseMap();
	}
	manager->orderIsDone(ordersInBasket);
	ordersInBasket.clear();

	while (pause == true) {}
}

int PickerRobot::getBasketSize() const
{
	return basketSize;
}

int PickerRobot::getNrItemsInBasket() const
{
	return itemsInBasket;
}

int PickerRobot::getTime() const
{
	return timer;
}

void PickerRobot::emergency_stop(bool stop)
{
	this->stop = stop;
}

void PickerRobot::pauseRobot(bool pause)
{
	this->pause = pause;
}

bool PickerRobot::sendCommand(const char c)
{
	char message[5] = "F";
	// Stop robot in case of emergency
	while (stop == true) {
		if (serial.ReadDataWaiting() > 0) {
			serial.ReadData(message, 1);
			if (message[0] == 'R') { stop = false; }
		}
	}

#if SERIAL
	if (serial.IsOpened()) {
		
		mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize), BASKET_NLINE, BASKET_NCOL);

		assert(serial.SendData(c));
		mapper->updateWarehouseMap(c);

		Sleep(S_TIME);
		if (serial.ReadDataWaiting() > 0) {
			serial.ReadData(message, 1);
			if (message[0] == 'E') { mapper->printLog(LOG_ERROR, "SERIAL COMM ERROR"); }
			else if (message[0] == 'S') {
				stop = true;
			}
			else if (message[0] == 'P') {
				pause = true;
			}
		}
	}
	else {
		mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize), BASKET_NLINE, BASKET_NCOL);
		mapper->updateWarehouseMap(c);
		Sleep(S_TIME);
	}
#else
	mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize), BASKET_NLINE, BASKET_NCOL);
	mapper->updateWarehouseMap(c);
	Sleep(S_TIME);
#endif
	timer += MOVE_TIME;

	return true;
}