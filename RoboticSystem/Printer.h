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

using namespace std; 

class Printer
{
public:
	static Printer* getInstance();

	//Printer(const Printer& printer);
	string current_time();
	void printlog(string warehouseID, string message);
	int addWindow(Warehouse warehouse, int offset);
	int addWindow(string name, int offset);
	void refreshw(string warehouseID);
	void printString(string warehouseID, int y, int x, const char* message);
	//void drawHorLine(string warehouseID, int y, int x, chtype c, int n);
	//void drawVertLine(string warehouseID, int y, int x, chtype c, int n);
	void drawBoxes();
private:
	static Printer* instance;
	Printer();
	map<string, WINDOW*> windows;
	mutex printer_mutex;
	mutex log_mutex;

};

