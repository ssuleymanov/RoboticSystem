#pragma once
#ifndef MAPPER_H
#define MAPPER_H

#include "Point.h"
#include "Warehouse.h"


class Mapper{

public:
	Mapper();
	Mapper(Warehouse* warehouse, Point startingPoint, Point unloadingPoint);
	Mapper(const Mapper& map);	//copy constructor
	~Mapper();
	void setCompartmentPosition(Point compartmentPt);
	void printWarehouseMap();
	void updateWarehouseMap(char command);
	void resetMap();
	Point* getCurrentPosition();

private:
	Point currentPosition;
	Point startingPoint;
	Point unloadingPoint;
	Point compartmentPosition;
	char command;
	bool itemIsPicked;
	Warehouse* warehouse;
};

#endif // !MAPPER_H
