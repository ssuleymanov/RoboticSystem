#pragma once
#ifndef MAPPER_H
#define MAPPER_H

#include "Point.h"
#include "Warehouse.h"
#include "Printer.h"
#include <pdcwin.h>


class Mapper{

public:
	//! A constructor.
	/*!
	\param[in] wrhouse Pointer to a Warehouse
	*/
	Mapper(Warehouse* wrhouse);

	//! A copy constructor.
	Mapper(const Mapper& map);

	//! A deconstructor.
	~Mapper();

	//! A function to initialize the Mapper.
	/*!
	\param[in] startPt Starting position of the robot
	\param[in] unloadPt Unloading position of the robot
	\param[in] printr Pointer to the Printer
	*/
	void Initialize(Point startPt, Point unloadPt, Printer* printr);

	//! A function to print to a log file and/or console screen.
	/*!
	\param[in] type Log type
	\param[in] message Message to be logged
	*/
	void printLog(log_type type, string message);

	//! A function to print a string to a window and clear the line.
	/*!
	\param[in] message Message to be printed
	\param[in] y Y starting position of the string
	\param[in] x X starting position of the string
	*/
	void printString(string message, int y, int x);

	//! A function to set the compartment position of the product to pick
	/*!
	\param[in] compartmentPt Point of the product compartment
	*/
	void setCompartmentPosition(Point compartmentPt);

	//! A function to print a warehouse map
	void printWarehouseMap();

	//! A function to set the command for the next action
	/*!
	\param[in] command Action command
	*/
	void updateWarehouseMap(char command);

	//! A function to reset the map
	void resetMap();

	//! A function to get the current position of the PickerRobot.
	/*!
	\return Current position
	*/
	Point* getCurrentPosition();

	//! A function to get the warehouseID of the warehouse associated with the Mapper
	/*!
	\return pointer to a Warehouse
	*/
	Warehouse* getWarehouse();

private:
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
