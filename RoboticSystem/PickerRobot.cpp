#include "PickerRobot.h"

PickerRobot::PickerRobot(int basketSize) : basketSize(basketSize)
{
}

PickerRobot::~PickerRobot()
{
	//delete mapper;
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

		mapper->printWarehoueMap();
	}
	for (int i = 0; i < abs(y); i++) {
		cout << "Moving to => " << dest << endl;
		if (y > 0)
			sendCommand(UP);
		else
			sendCommand(DOWN);

		mapper->printWarehoueMap();
		
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
	mapper->printWarehoueMap();
}

void PickerRobot::validate()
{
	cout << "Validating an item" << endl;
	sendCommand(VALIDATE);
	mapper->printWarehoueMap();
}

void PickerRobot::store()
{
	cout << "Storing an item" << endl;
	sendCommand(STORE);
	mapper->printWarehoueMap();
}

void PickerRobot::unload()
{
	cout << "Unloading an item" << endl;
	sendCommand(UNLOAD);
	mapper->printWarehoueMap();
}

void PickerRobot::sendCommand(const char c)
{
	if (serial.IsOpened()) {
		char message[5] = "F";
		cout << "Send command => " << c << endl;

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