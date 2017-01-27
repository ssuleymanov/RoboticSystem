#include "Printer.h"

Printer* Printer::instance = 0;

Printer::Printer()
{
	logLine = 1;
}

string Printer::current_time() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", &timeinfo);
	string str(buffer);

	return str;
}

Printer * Printer::getInstance()
{
	if (instance == 0)
	{
		instance = new Printer();
	}
	return instance;
}

void Printer::printLog(log_type type,string warehouseID, string message)
{
	lock_guard<mutex> guard(log_mutex);
	WINDOW* win = windows["log"];

	if (type == LOG_ACTIONS) {
		string filename = "Logs\\log" + warehouseID + ".txt";
		ofstream myfile;
		myfile.open(filename, ios_base::app);
		myfile << current_time() << " - " << message << "\n";
		myfile.close();
		return;
	}
	else if (type == LOG_ERROR) {
		string m = "[" + warehouseID + "] " + message;
		cerr << current_time() << " - " << m << "\n";
		wattron(win, COLOR_PAIR(3) | A_BOLD);
		printString("log", logLine, TEXT_VOFFSET, m);
		wattroff(win, COLOR_PAIR(3) | A_BOLD);
	}
	else if (type == LOG_INFO) {
		string m = "[" + warehouseID + "] " + message;
		clog << current_time() << " - " << message << "\n";
		return;
		
	}
	else if (type == LOG_SCREEN) {
		string m = "[" + warehouseID + "] " + message;
		printString("log", logLine, TEXT_VOFFSET, m);
	}
	
	if (logLine < MAX_LOG_LINES) { logLine++; }
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	if (logLine >= MAX_LOG_LINES) { scroll(win); }
	box(win, 0, 0);
	wrefresh(win);
}


int Printer::addWindow(Warehouse warehouse, int offset)
{
	int height = MAX_WINDOW_HEIGHT;
	int width = warehouse.getCols() * MAPSPACE_WIDTH + MAPSPACE_WIDTH_OFFSET;
	if (width < MIN_WINDOW_WIDTH) {
		width = MIN_WINDOW_WIDTH;	
	}
	WINDOW* win = newwin(height, width, VMAP_OFFSET, offset);
	box(win, 0, 0);
	string m = "Warehouse " + warehouse.getWarehouseID();
	mvwaddstr(win, 1, TEXT_VOFFSET, m.c_str());
	wrefresh(win);
	windows.insert(pair<string,WINDOW*>(warehouse.getWarehouseID(),win));
	return width;
}

int Printer::addWindow(string name, int height, int width, int x, int y)
{
	WINDOW* win = newwin(height, width, y, x);
	scrollok(win,true);
	box(win, 0, 0);
	wrefresh(win);
	windows.insert(pair<string, WINDOW*>(name, win));
	return (width);
}


void Printer::refreshw(string warehouseID)
{
	lock_guard<mutex> guard(printer_mutex);
	wborder(windows[warehouseID], ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	box(windows[warehouseID], 0, 0);
	wrefresh(windows[warehouseID]);
}

void Printer::printMap(string warehouseID, int y, int x, string message)
{
	lock_guard<mutex> guard(printer_mutex);
	mvwaddstr(windows[warehouseID], y, x, message.c_str());
}

void Printer::printString(string warehouseID, int y, int x, string message)
{
	lock_guard<mutex> guard(printer_mutex);
	mvwaddstr(windows[warehouseID], y, x, message.c_str());
	int newX = x + message.size();
	int windowX = getmaxx(windows[warehouseID])-1;
	if (newX < windowX) {
		mvwaddstr(windows[warehouseID], y, newX, string(windowX-newX,' ').c_str());
	}
}

void Printer::drawBoxes()
{
	lock_guard<mutex> guard(printer_mutex);
	for (auto& window : windows) {
		box(window.second, 0, 0);
		wrefresh(window.second);
	}
}

void Printer::clearWindows()
{
	lock_guard<mutex> guard(printer_mutex);
	for (auto& window : windows) {
		wclear(window.second);
	}
}

