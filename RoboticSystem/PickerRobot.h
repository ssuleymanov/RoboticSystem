#pragma once
#include <list>
#include <vector>
#include <assert.h>
#include <fstream>
#include <string>
#include <sstream>

#include "Order.h"
#include "Serial.h"
#include "Point.h"
#include "Mapper.h"

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

//! An abstract robot class representing a picker robot. 
/*!
Allows to create instructions (robot commands) for a robot and send them through RS232. Also allows to set the parameters of serial port . 
*/

class PickerRobot 
{
public:
	PickerRobot(int basketSize);
	~PickerRobot();
	PickerRobot(const PickerRobot& robot);
	void moveTo(Point start, Point dest);			
	void setSerialParameters(int portNumber, int baudRate);
	void setMapper(Mapper *map);
	void startSerial();
	void pick();
	bool validate(Order order);
	void store(Order order);
	void unload();
	int getBasketSize();
	int getNrItemsInBasket();

private:
	int basketSize;
	int portNumber;
	int baudRate;
	CSerial serial;
	Mapper* mapper;
	void sendCommand(const char c);
	int itemsInBasket;
	vector<Order> ordersInBasket;

};