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
	string filename = "log" + warehouseID + ".txt";
	ofstream myfile;
	myfile.open(filename,ios_base::app);
	myfile << current_time() << " - " << message << "\n";
	myfile.close();
}

int Printer::addWindow(Warehouse warehouse, int offset)
{
	int height = warehouse.getRows() * 3 + 4;
	if (height > 50)
		height = 50;
	int width = warehouse.getCols() * 6;
	if (width < 30) {
		width = 30;
		
	}
	WINDOW* win = newwin(height, width, 0, offset);
	box(win, 0, 0);
	string m = "Warehouse " + warehouse.getWarehouseID();
	mvwaddstr(win, 1, 3, m.c_str());
	wrefresh(win);
	windows.insert(pair<string,WINDOW*>(warehouse.getWarehouseID(),win));
	return width;
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
		wattron(windows[warehouseID],A_STANDOUT);
		mvwaddstr(windows[warehouseID], y, x, message);
		wattroff(windows[warehouseID], A_STANDOUT);
	}
	else {
		mvwaddstr(windows[warehouseID], y, x, message);
	}

}

void Printer::drawHorLine(string warehouseID, int y, int x, chtype c, int n)
{
	lock_guard<mutex> guard(printer_mutex);
	mvwhline(windows[warehouseID],y,x,c,n);
}

void Printer::drawVertLine(string warehouseID, int y, int x, chtype c, int n)
{
	lock_guard<mutex> guard(printer_mutex);
	mvwvline(windows[warehouseID],y,x,c,n);
}

