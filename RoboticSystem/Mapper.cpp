#include "Mapper.h"
#include "Warehouse.h"

Mapper::Mapper()
{
	currentPosition = new Point(1, 1);
	itemIsPicked = new bool(false);

}

Mapper::Mapper(Warehouse wrhouse, Point start, Point unload) : warehouse(wrhouse),startingPoint(start),unloadingPoint(unload)
{
	int start_X = startingPoint.getX();
	int start_Y = warehouse.getRows() + 1 - startingPoint.getY();
	currentPosition = new Point(start_X, start_Y);

	command = 'A';

	itemIsPicked = new bool(false);
}

Mapper::Mapper(Mapper & map) : warehouse(map.warehouse)
{
	startingPoint = map.startingPoint;
	unloadingPoint = map.unloadingPoint;
	command = map.command;
	compartmentPosition = map.compartmentPosition;
	currentPosition = new Point(map.currentPosition->getX(), map.currentPosition->getY());
	itemIsPicked = new bool(*map.itemIsPicked);
}

Mapper::~Mapper()
{
	//delete currentPosition;
	//delete itemIsPicked;
}

void Mapper::setCompartmentPosition(Point compartmentPt)
{
	this->compartmentPosition = compartmentPt;
}

void Mapper::printWarehouseMap()
{
	int rows = warehouse.getRows();
	int cols = warehouse.getCols();

	int unload_X = unloadingPoint.getX();
	int unload_Y = rows + 1 - unloadingPoint.getY();


	if (command == 'U') {
		currentPosition->setY(currentPosition->getY() - 1);
	}
	else if (command == 'D') {
		currentPosition->setY(currentPosition->getY() + 1);
	}
	else if (command == 'R') {
		currentPosition->setX(currentPosition->getX() + 1);
	}
	else if (command == 'L') {
		currentPosition->setX(currentPosition->getX() - 1);
	}

	if (currentPosition->getX() == compartmentPosition.getX() && currentPosition->getY() == rows + 1 - compartmentPosition.getY()) {
		*itemIsPicked = true;
	}

	for (int j = 1; j <= rows; j++) {
		for (int i = 0; i < cols; i++) {
			cout << " __";
		}
		cout << endl;
		for (int i = 1; i <= cols + 1; i++) {

			if (i == currentPosition->getX() && j == currentPosition->getY()) {
				cout << "|" << "R" << setw(2);
			}
			else if (i == unload_X && j == unload_Y) {
				cout << "|" << "U" << setw(2);
			}
			else if (i == compartmentPosition.getX() && j == rows + 1 - compartmentPosition.getY()) {
				if (*itemIsPicked == false)
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

	Sleep(2000);
	system("cls");
}

void Mapper::updateWarehouseMap(char command)
{
	this->command = command;
}

void Mapper::resetMap()
{
	*itemIsPicked = false;
}

Point Mapper::getCurrentPosition()
{
	Point temp(currentPosition->getX(), warehouse.getRows() + 1 - currentPosition->getY());
	return temp;
}
