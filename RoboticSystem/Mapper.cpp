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
	int unload_Y = rows - unloadingPoint.getY() + 1;


	if (command == 'U') {
		currentPosition.setY(currentPosition.getY() + 1);
		printer->printString(warehouse->getWarehouseID(), MOVE_NLINE,MOVE_NCOL,"Moving Up            ");
	}
	else if (command == 'D') {
		currentPosition.setY(currentPosition.getY() - 1);
		printer->printString(warehouse->getWarehouseID(), MOVE_NLINE, MOVE_NCOL, "Moving Down             ");
	}
	else if (command == 'R') {
		currentPosition.setX(currentPosition.getX() + 1);
		printer->printString(warehouse->getWarehouseID(), MOVE_NLINE, MOVE_NCOL, "Moving Right             ");
	}
	else if (command == 'L') {
		currentPosition.setX(currentPosition.getX() - 1);
		printer->printString(warehouse->getWarehouseID(), MOVE_NLINE, MOVE_NCOL, "Moving Left               ");
	}

	if (currentPosition.getX() == compartmentPosition.getX() && currentPosition.getY() == compartmentPosition.getY()) {
		itemIsPicked = true;
	}

	for (int j = 0; j <= rows + 1; j++) {
		int nlines = (j*2) + 5;
		if (j == 0) {	
			printer->printString(warehouse->getWarehouseID(),nlines+1,4, string(1, 201).c_str());
			for (int i = 1; i <= cols; i++) {
				int ncols = (i * 4)+1;
				printer->printString(warehouse->getWarehouseID(), nlines+1, ncols, string(3, 205).c_str());
				if (i < cols) {
					printer->printString(warehouse->getWarehouseID(), nlines+1, ncols+3, string(1, 203).c_str());
				}
			}
			printer->printString(warehouse->getWarehouseID(), nlines+1, (cols*4)+4, string(1, 187).c_str());
		}
		else if (j == rows + 1) {
			printer->printString(warehouse->getWarehouseID(), nlines-1, 4, string(1, 200).c_str());
			for (int i = 1; i <= cols; i++) {
				int ncols = (i * 4)+1;
				printer->printString(warehouse->getWarehouseID(), nlines-1, ncols, string(3, 205).c_str());
				if (i<cols) {
					printer->printString(warehouse->getWarehouseID(), nlines-1, ncols+3, string(1, 202).c_str());
				}
			}
			printer->printString(warehouse->getWarehouseID(), nlines-1, (cols*4)+4, string(1, 188).c_str());
		}
		else {
			for (int i = 1; i <= cols; i++) {
				int ncols = (i * 4);
				printer->printString(warehouse->getWarehouseID(), nlines, ncols, string(1, 186).c_str());
				if (i == currentPosition.getX() && j == rows - currentPosition.getY() + 1) {
					printer->printString(warehouse->getWarehouseID(), nlines, ncols+1, " R ");
				}
				else if (i == unload_X && j == unload_Y) {
					printer->printString(warehouse->getWarehouseID(), nlines, ncols+1, " U ");
				}
				else if (i == compartmentPosition.getX() && j == rows - compartmentPosition.getY() + 1) {
					if (itemIsPicked == false)
						printer->printString(warehouse->getWarehouseID(), nlines, ncols + 1, " P ");
					else {
						printer->printString(warehouse->getWarehouseID(), nlines, ncols + 1, "   ");
					}
				}
				else {
					//string comp = to_string((rows-j) + 1 + (i-1)*rows)+ "  ";
					//printer->printString(warehouse->getWarehouseID(), nlines, ncols + 1, comp.c_str());
					printer->printString(warehouse->getWarehouseID(), nlines, ncols+1, "   ");
				}
			}
			printer->printString(warehouse->getWarehouseID(), nlines, (cols*4)+4, string(1, 186).c_str());
			if (j < rows) {
				printer->printString(warehouse->getWarehouseID(), nlines+1, 4, string(1, 204).c_str());
				for (int i = 1; i <= cols; i++) {
					int ncols = (i * 4) + 1;
					printer->printString(warehouse->getWarehouseID(), nlines+1, ncols, string(3, 205).c_str());
					if (i < cols) {
						printer->printString(warehouse->getWarehouseID(), nlines+1, ncols+3, string(1, 206).c_str());
					}
				}
				printer->printString(warehouse->getWarehouseID(), nlines+1, (cols*4)+4, string(1, 185).c_str());
			}
		}
	}
	printer->refreshw(warehouse->getWarehouseID());

	Sleep(500);
}

void Mapper::updateWarehouseMap(char command)
{
	this->command = command;
}

void Mapper::printLog(string message) {
	printer->printlog(warehouse->getWarehouseID(), message);
}

void Mapper::printString(string message,int y, int x) {
	printer->printString(warehouse->getWarehouseID(), y, x, message.c_str());
	printer->refreshw(warehouse->getWarehouseID());
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

Warehouse * Mapper::getWarehouse()
{
	return warehouse;
}
