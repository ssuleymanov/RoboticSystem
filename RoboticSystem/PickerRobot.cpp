#include "PickerRobot.h"

PickerRobot::PickerRobot(int basketSize) : basketSize(basketSize)
{
	itemsInBasket = 0;
}

PickerRobot::~PickerRobot()
{
}

void PickerRobot::moveTo(Point start, Point dest)
{
	int x = 0, y = 0;
	x = dest.getX() - start.getX();
	y = dest.getY() - start.getY();
	for (int i = 0; i < abs(x); i++) {
		cout << "Moving to => " << dest << endl;
		if (x > 0)
			sendCommand(RIGHT);
		else
			sendCommand(LEFT);

		mapper->printWarehouseMap();
	}
	for (int i = 0; i < abs(y); i++) {
		cout << "Moving to => " << dest << endl;
		if (y > 0)
			sendCommand(UP);
		else
			sendCommand(DOWN);

		mapper->printWarehouseMap();
		
	}

	clog << "\tMoving from " << start.getX() << "," << start.getY();
	clog << " to " << dest.getX() << "," << dest.getY() << endl;
}


void PickerRobot::setSerialParameters(int portNumber, int baudRate)
{
	this->portNumber = portNumber;
	this->baudRate = baudRate;
}

void PickerRobot::setMapper(Mapper* map)
{
	mapper = map;
}

void PickerRobot::startSerial()
{
	if (serial.Open(portNumber, baudRate)) {
		cout << "Port opened succesfully.." << endl;
	}
	else {
		cout << "Failed to open port..!" << endl;
	}
}

void PickerRobot::pick()
{
	cout << "Picking up an item" << endl;
	sendCommand(PICK);
	mapper->printWarehouseMap();
}

bool PickerRobot::validate(Order order)
{
	cout << "Validating an item" << endl;
	sendCommand(VALIDATE);

	string line;
	std::ifstream file("validate.txt");
	while (std::getline(file, line)) {
		std::istringstream iss(line);
	
	
	}
	mapper->printWarehouseMap();
}

void PickerRobot::store()
{
	cout << "Storing an item" << endl;
	sendCommand(STORE);
	mapper->printWarehouseMap();
	itemsInBasket++;
}

void PickerRobot::unload()
{
	cout << "Unloading items" << endl;
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