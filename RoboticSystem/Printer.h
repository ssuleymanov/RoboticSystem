#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include "../pdcurses/win32/pdcwin.h"
//#include <pdcwin.h>
#include <thread>
#include <string>
#include <iterator>
#include <mutex>
#include <ctime>

#include "Warehouse.h"

#define ACTION_NLINE 3
#define ACTION_NCOL 3
#define MOVE_NLINE 4
#define MOVE_NCOL 3
#define BASKET_NLINE 5
#define BASKET_NCOL 3

#define VMAP_OFFSET 1

enum log_type
{
	LOG_ERROR,
	LOG_INFO,
	LOG_ACTIONS
};

using namespace std; 

class Printer
{
public:
	static Printer* getInstance();

	string current_time();
	void printLog(log_type type, string warehouseID, string message);
	int addWindow(Warehouse warehouse, int offset);
	int addWindow(string name, int height, int width, int x, int y);
	void refreshw(string warehouseID);
	void printMap(string warehouseID, int y, int x, string message);
	void printString(string warehouseID, int y, int x, string message);
	void drawBoxes();
private:
	static Printer* instance;
	int logLine;
	Printer();
	map<string, WINDOW*> windows;
	mutex printer_mutex;
	mutex log_mutex;

};

