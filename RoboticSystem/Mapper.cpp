#include "Mapper.h"

Mapper::Mapper()
{
}

Mapper::Mapper(Warehouse* wrhouse, Point start, Point unload) : warehouse(wrhouse),startingPoint(start),unloadingPoint(unload)
{
	int start_X = startingPoint.getX();
	int start_Y = warehouse.getRows() + 1 - startingPoint.getY();
	currentPosition = Point(start_X, start_Y);

	command = 'A';

	itemIsPicked = false;
	command = 'A';		// initialize to 'A', can be any character 
}

Mapper::Mapper(const Mapper& map) : warehouse(map.warehouse)
{
	startingPoint = map.startingPoint;
	unloadingPoint = map.unloadingPoint;
	command = map.command;
	compartmentPosition = map.compartmentPosition;
	currentPosition = map.currentPosition;
	itemIsPicked = map.itemIsPicked;
}

Mapper::~Mapper()
{
}

void Mapper::setCompartmentPosition(Point compartmentPt)
{
	compartmentPosition = compartmentPt;
}

void Mapper::printWarehouseMap()
{
	int rows = warehouse->getRows();
	int cols = warehouse->getCols();

	int unload_X = unloadingPoint.getX();
	int unload_Y = unloadingPoint.getY();


	if (command == 'U') {
		currentPosition.setY(currentPosition.getY() + 1);
	}
	else if (command == 'D') {
		currentPosition.setY(currentPosition.getY() - 1);
	}
	else if (command == 'R') {
		currentPosition.setX(currentPosition.getX() + 1);
	}
	else if (command == 'L') {
		currentPosition.setX(currentPosition.getX() - 1);
	}

	if (currentPosition.getX() == compartmentPosition.getX() && currentPosition.getY() == compartmentPosition.getY()) {
		itemIsPicked = true;
	}

	for (int j = rows; j >= 1; j--) {
		for (int i = 0; i < cols; i++) {
			cout << " __";
		}
		cout << endl;
		for (int i = 1; i <= cols + 1; i++) {

			if (i == currentPosition.getX() && j == currentPosition.getY()) {
				cout << "|" << "R" << setw(2);
			}
			else if (i == unload_X && j == unload_Y) {
				cout << "|" << "U" << setw(2);
			}
			else if (i == compartmentPosition.getX() && j == compartmentPosition.getY()) {
				if (itemIsPicked == false)
					cout << "|" << "P" << setw(2);
				else
					cout << "|" << setw(3);
			}
			else {
				cout << "|" << setw(3);
			}
		}
		cout << endl;
	}
	cout << endl;

	Sleep(1000);
	system("cls");
}

void Mapper::updateWarehouseMap(char command)
{
	this->command = command;
}

void Mapper::resetMap()
{
	itemIsPicked = false;
}

Point* Mapper::getCurrentPosition()
{
	//Point temp(currentPosition.getX(), warehouse->getRows() + 1 - currentPosition.getY());
	return &currentPosition;
}