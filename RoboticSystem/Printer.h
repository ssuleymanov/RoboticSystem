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

#define ACTION_NLINE MAP_OFFSET - 2
#define ACTION_NCOL 3
#define MOVE_NLINE MAP_OFFSET - 1
#define MOVE_NCOL 3
#define BASKET_NLINE MAP_OFFSET
#define BASKET_NCOL 3
#define ORDER_NLINE MAP_OFFSET - 3
#define ORDER_NCOL 3
#define PROGRESS_NLINE MAP_OFFSET - 4
#define PROGRESS_NCOL 3

#define VMAP_OFFSET 1
#define MAP_OFFSET 7

#define MAX_HEIGHT 55
#define MIN_WIDTH 32 	

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

