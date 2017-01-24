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
		mapper->printLog(LOG_INFO,"Port " + to_string(portNumber) + " opended succesfully..\n");
	}
	else {
		clog << "Failed to open port " << portNumber << "..!" << endl;
		mapper->printLog(LOG_ERROR,"Failed to open port " + to_string(portNumber) + "..!\n");
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
	mapper->printLog(LOG_ERROR,"Validation ERROR - " + order.productID);
	mapper->printString("Wrong product ID.", ACTION_NLINE, ACTION_NCOL);
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
		//for (int i = 0; i < it->quantity; i++) {
		itemsInBasket--;
		sendCommand(UNLOAD);
		mapper->printWarehouseMap();
			//TODO add time for each unload
		//}
		manager->orderIsDone(*it);
		mapper->getWarehouse()->getUnloadedOrders().push_back(*it);	
	}
	ordersInBasket.clear();

	while (pause == true) {}

	//itemsInBasket = 0;
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
	while (stop == true) {}
#if SERIAL
	if (serial.IsOpened()) {
		char message[5] = "F";
		mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize), BASKET_NLINE, BASKET_NCOL);

		assert(serial.SendData(c));
		mapper->updateWarehouseMap(c);

		bool recieved = false;
		int counter = 0;
		while (!recieved) {
			if (serial.ReadDataWaiting() > 0) {
				serial.ReadData(message, 1);
				if (message == "E") { mapper->printLog(LOG_ERROR, "SERIAL COMM ERROR"); }
				recieved = true;
			}
			if (counter > 2000) {
				mapper->printLog(LOG_ERROR, "SERIAL COMM TIMEOUT\n");
				recieved = true;
			}
			Sleep(1); 
			counter++;
		}
	}
#else
	mapper->printString("Basket: " + to_string(itemsInBasket) + "/" + to_string(basketSize), BASKET_NLINE, BASKET_NCOL);
	mapper->updateWarehouseMap(c);
	Sleep(100);
#endif
	timer += MOVE_TIME;

	return true;
}