#pragma once
#include <iostream>
#include <fstream>
#include <map>
#include <pdcwin.h>
#include <thread>
#include <string>
#include <iterator>
#include <mutex>
#include <ctime>

#include "Warehouse.h"

using namespace std; 

class Printer
{
public:
	static Printer* getInstance();

	//Printer(const Printer& printer);
	string current_time();
	void printlog(string warehouseID, string message);
	int addWindow(Warehouse warehouse, int offset);
	void refreshw(string warehouseID);
	void printString(string warehouseID, int y, int x, const char* message);
	void makeBox(string warehouseID);
private:
	static Printer* instance;
	Printer();
	map<string, WINDOW*> windows;
	mutex printer_mutex;
	mutex log_mutex;

};

