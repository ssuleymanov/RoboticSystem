#include "Printer.h"

Printer* Printer::instance = 0;

Printer::Printer()
{
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

void Printer::printlog(string warehouseID, string message)
{
	lock_guard<mutex> guard(printer_mutex);
	string filename = "Logs\\log" + warehouseID + ".txt";
	ofstream myfile;
	myfile.open(filename,ios_base::app);
	myfile << current_time() << " - " << message << "\n";
	myfile.close();
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

int Printer::addWindow(string name, int offset)
{
	WINDOW* win = newwin(3, offset, 0, 0);
	box(win, 0, 0);
	wrefresh(win);
	windows.insert(pair<string, WINDOW*>(name, win));
	return offset;
}



void Printer::refreshw(string warehouseID)
{
	lock_guard<mutex> guard(printer_mutex);
	wrefresh(windows[warehouseID]);
}

void Printer::printString(string warehouseID, int y, int x, const char* message)
{
	lock_guard<mutex> guard(printer_mutex);
	if (message == " R ") {
		wattron(windows[warehouseID],COLOR_PAIR(2));
		mvwaddstr(windows[warehouseID], y, x, message);
		wattroff(windows[warehouseID], COLOR_PAIR(2));
	}
	else {
		mvwaddstr(windows[warehouseID], y, x, message);
	}
	//mvwaddstr(windows[warehouseID], y, x, message);
}

void Printer::drawBoxes()
{
	for (auto& window : windows) {
		box(window.second, 0, 0);
	}
}

//void Printer::drawHorLine(string warehouseID, int y, int x, chtype c, int n)
//{
//	lock_guard<mutex> guard(printer_mutex);
//	mvwhline(windows[warehouseID],y,x,c,n);
//}
//
//void Printer::drawVertLine(string warehouseID, int y, int x, chtype c, int n)
//{
//	lock_guard<mutex> guard(printer_mutex);
//	mvwvline(windows[warehouseID],y,x,c,n);
//}

