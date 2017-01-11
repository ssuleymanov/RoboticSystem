#include "Mapper.h"

Mapper::Mapper(Warehouse* wrhouse) : warehouse(wrhouse)
{
	printer = Printer::getInstance();
	currentPosition = Point(1, 1);
	itemIsPicked = false;
	command = 'A';
}

//Mapper::Mapper(Warehouse* wrhouse, Point start, Point unload, Printer* printer) : warehouse(wrhouse),startingPoint(start),unloadingPoint(unload), printer(printer)
//{
//	Printer* p = Printer::getInstance();
//	Sleep(500);
//	int start_X = startingPoint.getX();
//	int start_Y = warehouse->getRows() + 1 - startingPoint.getY();
//	currentPosition = startingPoint;
//
//	command = 'A';
//
//	itemIsPicked = false;
//}

Mapper::Mapper(const Mapper& map)
{
	warehouse = map.warehouse;
	startingPoint = map.startingPoint;
	unloadingPoint = map.unloadingPoint;
	command = map.command;
	compartmentPosition = map.compartmentPosition;
	currentPosition = map.currentPosition;
	itemIsPicked = map.itemIsPicked;
	printer = map.printer;
}

Mapper::~Mapper()
{
	//delete currentPosition;
	//delete itemIsPicked;
}

void Mapper::Initialize(Point startPt, Point unloadPt, Printer * printr)
{
	startingPoint = startPt;
	unloadingPoint = unloadPt;
	printer = printr;
	currentPosition = startingPoint;
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
		printer->printString(warehouse->getWarehouseID(), 3,3,"Moving Up            ");
	}
	else if (command == 'D') {
		currentPosition.setY(currentPosition.getY() - 1);
		printer->printString(warehouse->getWarehouseID(), 3, 3, "Moving Down             ");
	}
	else if (command == 'R') {
		currentPosition.setX(currentPosition.getX() + 1);
		printer->printString(warehouse->getWarehouseID(), 3, 3, "Moving Right             ");
	}
	else if (command == 'L') {
		currentPosition.setX(currentPosition.getX() - 1);
		printer->printString(warehouse->getWarehouseID(), 3, 3, "Moving Left               ");
	}

	if (currentPosition.getX() == compartmentPosition.getX() && currentPosition.getY() == compartmentPosition.getY()) {
		itemIsPicked = true;
	}

	string lineHor = "";
	for (int i = 0; i < cols; i++) {
		lineHor += " __";
	}

	for (int j = rows; j >= 1; j--) {
		int nlines = (j * 2) + 5;
		printer->printString(warehouse->getWarehouseID(), nlines-1, 3, lineHor.c_str());
		for (int i = 1; i <= cols + 1; i++) {
			int ncols = (i * 3);	
			if (i == currentPosition.getX() && j == currentPosition.getY()) {
				printer->printString(warehouse->getWarehouseID(), nlines, ncols, "|R");
			}
			else if (i == unload_X && j == unload_Y) {
				printer->printString(warehouse->getWarehouseID(), nlines, ncols, "|U");
			}
			else if (i == compartmentPosition.getX() && j == compartmentPosition.getY()) {
				if (itemIsPicked == false) {
					printer->printString(warehouse->getWarehouseID(), nlines, ncols, "|P");
				}
				else {
					printer->printString(warehouse->getWarehouseID(), nlines, ncols, "| ");
				}
			}
			else {
				printer->printString(warehouse->getWarehouseID(), nlines, ncols, "| ");
			}
		}
	}
	printer->printString(warehouse->getWarehouseID(), (rows + 1) * 2 + 4, 3, lineHor.c_str());

	printer->refreshw(warehouse->getWarehouseID());

	Sleep(100);
}

void Mapper::updateWarehouseMap(char command)
{
	this->command = command;
}

void Mapper::printLog(string message) {
	printer->printString(warehouse->getWarehouseID(), 3, 3, message.c_str());
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