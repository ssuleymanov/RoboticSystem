#include "Printer.h"

Printer* Printer::instance = 0;

Printer::Printer()
{
	logLine = 1;
}

//Printer::Printer(const Printer& printer) {
//	windows = printer.windows;
//}

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
		cerr << m << "\n";
		wattron(win, COLOR_PAIR(3) | A_BOLD);
		printString("log", logLine, 1, m);
		wattroff(win, COLOR_PAIR(3) | A_BOLD);
	}
	else if (type == LOG_INFO) {
		string m = "[" + warehouseID + "] " + message;
		clog << message << "\n";
		return;
		//printString("log", logLine, 1, m);
	}
	
	if (logLine < 48) { logLine++; }
	wborder(win, ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ');
	if (logLine >= 48) { scroll(win); }
	box(win, 0, 0);
	wrefresh(win);
}


int Printer::addWindow(Warehouse warehouse, int offset)
{
	int height = warehouse.getRows() * 2 + 6 + MAP_OFFSET;
	if (height > MAX_HEIGHT)
		height = MAX_HEIGHT;
	int width = warehouse.getCols() * 4 + 12;
	if (width < MIN_WIDTH) {
		width = MIN_WIDTH;	
	}
	WINDOW* win = newwin(height, width, VMAP_OFFSET, offset);
	box(win, 0, 0);
	string m = "Warehouse " + warehouse.getWarehouseID();
	mvwaddstr(win, 1, 3, m.c_str());
	wrefresh(win);
	windows.insert(pair<string,WINDOW*>(warehouse.getWarehouseID(),win));
	return width;
}

int Printer::addWindow(string name, int height, int width, int x, int y)
{
	WINDOW* win = newwin(height, width, VMAP_OFFSET, x);
	scrollok(win,true);
	box(win, 0, 0);
	wrefresh(win);
	windows.insert(pair<string, WINDOW*>(name, win));
	return (width+10);
}


void Printer::refreshw(string warehouseID)
{
	lock_guard<mutex> guard(printer_mutex);
	wrefresh(windows[warehouseID]);
}

void Printer::printMap(string warehouseID, int y, int x, string message)
{
	lock_guard<mutex> guard(printer_mutex);
	if (message == " R ") {
		wattron(windows[warehouseID], COLOR_PAIR(2));
		mvwaddstr(windows[warehouseID], y, x, message.c_str());
		wattroff(windows[warehouseID], COLOR_PAIR(2));
	}
	else {
		mvwaddstr(windows[warehouseID], y, x, message.c_str());
	}
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

