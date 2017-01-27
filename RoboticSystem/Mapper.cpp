#include "Mapper.h"

Mapper::Mapper(Warehouse* wrhouse) : warehouse(wrhouse)
{
	printer = Printer::getInstance();
	currentPosition = Point(1, 1);
	itemIsPicked = false;
	command = 'A';
}

Mapper::Mapper(const Mapper& map):
	warehouse(map.warehouse),
	startingPoint(map.startingPoint),
	unloadingPoint(map.unloadingPoint),
	command(map.command),
	compartmentPosition(map.compartmentPosition),
	currentPosition(map.currentPosition),
	itemIsPicked(map.itemIsPicked),
	printer(map.printer)
{
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

	int newXPosition = currentPosition.getX();
	int newYPosition = currentPosition.getY();
	string log_string;

	if (command == 'U') {
		newYPosition++;
		log_string = "Moving Up";
	}
	else if (command == 'D') {
		newYPosition--;
		log_string = "Moving Down";
	}
	else if (command == 'R') {
		newXPosition++;
		log_string = "Moving Right";
	}
	else if (command == 'L') {
		newXPosition--;
		log_string = "Moving Left";
	}

	currentPosition.setX(newXPosition);
	currentPosition.setY(newYPosition);
	printer->printString(warehouse->getWarehouseID(), MOVE_NLINE, MOVE_NCOL, log_string);

	if (newXPosition == compartmentPosition.getX() && newYPosition == compartmentPosition.getY()) {
		itemIsPicked = true;
	}

	for (int j = 0; j <= rows + 1; j++) {
		int nlines = (j*MAPSPACE_HEIGHT) + 1 + MAP_OFFSET;
		if (j == 0) {	//Print top part of map
			printer->printMap(warehouse->getWarehouseID(),nlines+1,4, string(1, 201).c_str());
			for (int i = 1; i <= cols; i++) {
				int ncols = (i * MAPSPACE_WIDTH)+1;
				printer->printMap(warehouse->getWarehouseID(), nlines+1, ncols, string(3, 205));
				if (i < cols) {
					printer->printMap(warehouse->getWarehouseID(), nlines+1, ncols+3, string(1, 203));
				}
			}
			printer->printMap(warehouse->getWarehouseID(), nlines + 1, (cols * 4) + 4, string(1, 187));
		}
		else if (j == rows + 1) { //Print bottom part of map 
			printer->printMap(warehouse->getWarehouseID(), nlines-1, 4, string(1, 200));
			for (int i = 1; i <= cols; i++) {
				int ncols = (i * MAPSPACE_WIDTH)+1;
				printer->printMap(warehouse->getWarehouseID(), nlines-1, ncols, string(3, 205));
				if (i<cols) {
					printer->printMap(warehouse->getWarehouseID(), nlines-1, ncols+3, string(1, 202));
				}
			}
			printer->printMap(warehouse->getWarehouseID(), nlines-1, (cols*4)+4, string(1, 188));
		}
		else {
			for (int i = 1; i <= cols; i++) { //Print rest of map
				int ncols = (i * MAPSPACE_WIDTH);
				printer->printMap(warehouse->getWarehouseID(), nlines, ncols, string(1, 186));
				if (i == currentPosition.getX() && j == rows - currentPosition.getY() + 1) {
					printer->printMap(warehouse->getWarehouseID(), nlines, ncols+1, " R ");
				}
				else if (i == unload_X && j == unload_Y) {
					printer->printMap(warehouse->getWarehouseID(), nlines, ncols+1, " U ");
				}
				else if (i == compartmentPosition.getX() && j == rows - compartmentPosition.getY() + 1) {
					if (itemIsPicked == false)
						printer->printMap(warehouse->getWarehouseID(), nlines, ncols + 1, " P ");
					else {
						printer->printMap(warehouse->getWarehouseID(), nlines, ncols + 1, "   ");
					}
				}
				else {
					printer->printMap(warehouse->getWarehouseID(), nlines, ncols+1, "   ");
				}
			}
			printer->printMap(warehouse->getWarehouseID(), nlines, (cols*4)+4, string(1, 186));
			if (j < rows) {
				printer->printMap(warehouse->getWarehouseID(), nlines+1, 4, string(1, 204));
				for (int i = 1; i <= cols; i++) {
					int ncols = (i * MAPSPACE_WIDTH) + 1;
					printer->printMap(warehouse->getWarehouseID(), nlines+1, ncols, string(3, 205));
					if (i < cols) {
						printer->printMap(warehouse->getWarehouseID(), nlines+1, ncols+3, string(1, 206));
					}
				}
				printer->printMap(warehouse->getWarehouseID(), nlines+1, (cols*4)+4, string(1, 185));
			}
		}
	}
	printer->refreshw(warehouse->getWarehouseID());
}

void Mapper::updateWarehouseMap(char command)
{
	this->command = command;
}

void Mapper::printLog(log_type type, string message) {
	printer->printLog(type,warehouse->getWarehouseID(), message);
}

void Mapper::printString(string message,int y, int x) {
	printer->printString(warehouse->getWarehouseID(), y, x, message);
	printer->refreshw(warehouse->getWarehouseID());
}

void Mapper::resetMap()
{
	itemIsPicked = false;
}

Point* Mapper::getCurrentPosition()
{
	return &currentPosition;
}

Warehouse * Mapper::getWarehouse()
{
	return warehouse;
}
