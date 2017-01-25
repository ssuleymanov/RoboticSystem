#pragma once
#include <list>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>
#include <sstream>
#include <algorithm>

#include "Order.h"
#include "Serial.h"
#include "Point.h"
#include "Mapper.h"
//#include "Printer.h"

#define SERIAL true

#define VALIDATE 'V'
#define PICK 'P'
#define STORE 'S'
#define CHECK 'C'
#define RIGHT 'R'
#define LEFT 'L'
#define UP 'U'
#define DOWN 'D'
#define MOVE 'M'
#define STOP 'X'
#define UNLOAD 'T'

#define MOVE_TIME 1;

//! An abstract robot class representing a picker robot. 
/*!
Allows the operation of a picking robot through RS232. 
*/
class Manager;

class PickerRobot 
{
public:
	//! A constructor.
	/*!
	\param[in] basketSize basket size of the robot (amount of products that the robot can carry at once)
	\param[in] &manager reference of the manager class that manages this PickerRobot
	*/
	PickerRobot(int basketSize, Manager &manager);
	//! A destructor.
	~PickerRobot();

	//! A copy constructor.
	PickerRobot(const PickerRobot& robot);

	//! A function to move a robot.
	/*!
	\param[in] start starting location of the robot
	\param[in] dest destination of movement
	*/
	void moveTo(Point start, Point dest);	

	//! A function to set the serial parameters.
	/*!
	\param[in] portNumber port number
	\param[in] baudRate baudrate
	*/
	void setSerialParameters(int portNumber, int baudRate);

	//! A function to set the serial parameters.
	/*!
	\param[in] *map pointer of a mapper
	*/
	void setMapper(Mapper *map);

	//! Start serial communication.
	void startSerial();

	//! Pick an item.
	void pick();

	//! A function to validate the productID of the order to be picked
	/*!
	\param[in] order Order to validate
	\return returns true if correct, otherwise false
	*/
	bool validate(Order order);

	//! A function to store a picked order inside a basket.
	/*!
	\param[in] order Order to store
	*/
	void store(Order order);

	//! Unload orders currently in the basket.
	void unload();

	//! A function to get the basket size of the PickerRobot
	/*!
	\return the size of the basket
	*/
	int getBasketSize();

	//! A function to get amount of items in the basket
	/*!
	\return number of items in the basket
	*/
	int getNrItemsInBasket();

	//! A function to get the working time of the PickerRobot
	/*!
	\return the number of seconds this robot has worked
	*/
	int getTime();

	//! A function to stop or resume the PickerRobot.
	/*!
	\param[in] stop set to true to stop or false to resume the PickerRobot
	*/
	void emergency_stop(bool stop);

	//! A function to pause or resume the PickerRobot.
	/*!
	\param[in] pause set to true to pause or false to resume the PickerRobot
	*/
	void pauseRobot(bool pause);

private:
	int basketSize;
	int portNumber;
	int baudRate;
	CSerial serial;
	Mapper* mapper;
	Manager* manager;
	bool sendCommand(const char c);
	int itemsInBasket;
	vector<Order> ordersInBasket;
	int timer;
	bool stop;
	bool pause;

};