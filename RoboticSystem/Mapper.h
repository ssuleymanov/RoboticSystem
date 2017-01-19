#pragma once
#ifndef MAPPER_H
#define MAPPER_H

#include "Point.h"
#include "Warehouse.h"
#include "Printer.h"
#include <pdcwin.h>


class Mapper{

public:
	Mapper(Warehouse* wrhouse);
	//Mapper(Warehouse* warehouse, Point startingPoint, Point unloadingPoint, Printer* printer);
	Mapper(const Mapper& map);	//copy constructor
	~Mapper();
	void Initialize(Point startPt, Point unloadPt, Printer* printr);
	void printLog(log_type type, string message);
	void printString(string message, int y, int x);
	void setCompartmentPosition(Point compartmentPt);
	void printWarehouseMap();
	void updateWarehouseMap(char command);
	void resetMap();
	Point* getCurrentPosition();
	Warehouse* getWarehouse();

private:
	//WINDOW* win;
	Printer* printer;
	Point currentPosition;
	Point startingPoint;
	Point unloadingPoint;
	Point compartmentPosition;
	char command;
	bool itemIsPicked;
	Warehouse* warehouse;
};

#endif // !MAPPER_H
