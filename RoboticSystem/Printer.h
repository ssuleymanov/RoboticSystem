#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include "../pdcurses/win32/pdcwin.h"
#include <thread>
#include <string>
#include <iterator>
#include <mutex>
#include <ctime>

#include "Warehouse.h"

#define PROGRESS_NLINE MAP_OFFSET - 4
#define PROGRESS_NCOL 3
#define ORDER_NLINE MAP_OFFSET - 3
#define ORDER_NCOL 3
#define ACTION_NLINE MAP_OFFSET - 2
#define ACTION_NCOL 3
#define MOVE_NLINE MAP_OFFSET - 1
#define MOVE_NCOL 3
#define BASKET_NLINE MAP_OFFSET
#define BASKET_NCOL 3



#define VMAP_OFFSET 1
#define MAP_OFFSET 7

#define MAX_HEIGHT 55
#define MIN_WIDTH 32 	

#define S_TIME 100

enum log_type
{
	LOG_ERROR,
	LOG_INFO,
	LOG_ACTIONS,
	LOG_SCREEN
};

using namespace std; 

//! A singleton class for printing to the screen and log files. 
/*!
	Allows the processing of an orderlist and calculates the optimal path for picking multiple orders.
*/
class Printer
{
public:
	//! Function to get an instance of a Printer.
	/*!
	\return Printer instance
	*/
	static Printer* getInstance();

	//! Function to get a string of the current date and time.
	/*!
	\return Date and Time in the format day-month-year hour:minutes:seconds
	*/
	string current_time();

	//! A function to print to a log file and/or console screen.
	/*!
	\param[in] type Log type
	\param[in] warehouseID Name of the warehouse this message is from
	\param[in] message Message to be logged
	*/
	void printLog(log_type type, string warehouseID, string message);

	//! A function to add a new window to the console screen with the size of a Warehouse map.
	/*!
	\param[in] warehouse Warehouse for which this window is created
	\param[in] offset X starting position of the window
	*/
	int addWindow(Warehouse warehouse, int offset);

	//! A function to add a new window to the console screen.
	/*!
	\param[in] name Name used to acces the window
	\param[in] height Height of the window in number of lines
	\param[in] width Width of the window in number of collumns
	\param[in] x X starting position of the window
	\param[in] y Y starting position of the window
	*/
	int addWindow(string name, int height, int width, int x, int y);

	//! A function to refresh a window
	/*!
	\param[in] warehouseID Name of the window to be refreshed
	*/
	void refreshw(string warehouseID);

	//! A function to print a string to a window.
	/*!
	\param[in] warehouseID Name of the window to be printed on
	\param[in] y Y starting position of the message
	\param[in] x X starting position of the message
	\param[in] message Message to be printed
	*/
	void printMap(string warehouseID, int y, int x, string message);

	//! A function to print a string to a window and clear the line.
	/*!
	\param[in] warehouseID Name of the window to be printed on
	\param[in] y Y starting position of the string
	\param[in] x X starting position of the string
	\param[in] message Message to be printed
	*/
	void printString(string warehouseID, int y, int x, string message);

	//! A function to draw boxes arround each window
	void drawBoxes();

	//! A function to clear all windows
	void clearWindows();

private:
	static Printer* instance;
	int logLine;
	Printer();
	map<string, WINDOW*> windows;
	mutex printer_mutex;
	mutex log_mutex;

};

