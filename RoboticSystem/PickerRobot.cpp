#include "PickerRobot.h"

PickerRobot::PickerRobot(int basketSize) : basketSize(basketSize)
{
	itemsInBasket = 0;
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
itemsInBasket(robot.itemsInBasket)
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
	//cout << "Picking up an item" << endl;
	sendCommand(PICK);
	mapper->printWarehouseMap();
}

void PickerRobot::validate()
{
	mapper->printString("Validating an item        ", ACTION_NLINE, ACTION_NCOL);
	//cout << "Validating an item" << endl;
	sendCommand(VALIDATE);
	mapper->printWarehouseMap();
}

void PickerRobot::store()
{
	mapper->printString("Storing an item         ", ACTION_NLINE, ACTION_NCOL);
	//cout << "Storing an item" << endl;
	sendCommand(STORE);
	mapper->printWarehouseMap();
	itemsInBasket++;
}

void PickerRobot::unload()
{
	mapper->printString("Unloading items         ", ACTION_NLINE, ACTION_NCOL);
	// << "Unloading items" << endl;
	sendCommand(UNLOAD);
	mapper->printWarehouseMap();
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

void PickerRobot::sendCommand(const char c)
{
	if (serial.IsOpened()) {
		char message[5] = "F";
		cout << "Send command => " << c << endl;
		cout << "Number of items in the basket: " << itemsInBasket << endl;

		assert(serial.SendData(c));
		mapper->updateWarehouseMap(c);

		if (serial.ReadDataWaiting() > 0) {
			serial.ReadData(message, 1);
			cout << "ERROR ON: " << message << endl;
		}
	}
	else {
		mapper->updateWarehouseMap(c);
	}
}