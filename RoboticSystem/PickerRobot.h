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
Allows to create instructions (robot commands) for a robot and send them through RS232. Also allows to set the parameters of serial port . 
*/
class Manager;

class PickerRobot 
{
public:
	PickerRobot(int basketSize, Manager &manager);
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
	int getTime();
	void emergency_stop(bool stop);
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