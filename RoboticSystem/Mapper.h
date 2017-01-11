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
	void printLog(string message);
	void setCompartmentPosition(Point compartmentPt);
	void printWarehouseMap();
	void updateWarehouseMap(char command);
	void resetMap();
	Point* getCurrentPosition();
	//void setWindow(Printer* printer, int offset);
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
