#include "CollectorRobot.h"

CollectorRobot::CollectorRobot()
{
}

CollectorRobot::~CollectorRobot()
{
}

CollectorRobot::CollectorRobot(LoadingDock& ld, string filename)
{
	this->loadingDock = &ld;
	this->currentPoint = LOADINGDOCK;
	ready = false;
	nrItemsInBasket = 0;
	totalTime = 0;
	warehouseIDs.push_back(LOADINGDOCK);

	ifstream file(filename);
	string path;
	int time;
	string value;

	getline(file, value); //Skip Header
	while (getline(file, value)) {
		stringstream ss(value);
		ss >> path >> time;
		path_times[path] = time;
	}
}

void CollectorRobot::InitCollector(int basketsize, int baudrate, int portnumber)
{
	this->basketSize = basketsize;
	this->baudRate = baudrate;
	this->portNumber = portnumber;
}

void CollectorRobot::startRobot(Printer* printr)
{
	printer = printr;
	printer->printString(COLLECTOR_WINDOW, 1, TEXT_VOFFSET, "Collector");
	//mvwaddstr(win, 1, TEXT_VOFFSET, m.c_str());
	printMap(currentPoint);
	if (serial.Open(portNumber, baudRate)) {
		printer->printLog(LOG_INFO, COLLECTOR_S, "Port " + to_string(portNumber) + " opened succesfully..");
	}
	else {
		printer->printLog(LOG_ERROR, COLLECTOR_S, "Failed to open port " + to_string(portNumber) + " ..!");
	}

	while (true) {
		if (ordersReady.size() > 0) {
			string bestWarehouse;
			int bestSize = 0;
			for (auto orders : ordersReady) {
				if (orders.second.size() > bestSize) {
					bestWarehouse = orders.first;
					bestSize = orders.second.size();
				}
			}
			if (bestSize > 0){ collectOrder(bestWarehouse); }
			if (bestSize == 0 && ready) { break; }
		}
		Sleep(100);
		printMap(currentPoint);
	}
	loadingDock->RemoveInvalidOrders();
	printer->printString(COLLECTOR_WINDOW, MOVE_NLINE, MOVE_NCOL, "Time = " + to_string(totalTime) + " sec");
}

void CollectorRobot::addWarehouseID(string warehouseID)
{
	warehouseIDs.push_back(warehouseID);
}

void CollectorRobot::addOrder(Order order)
{
	lock_guard<mutex> lock(order_mutex);
	ordersReady[order.warehouseID].push_back(order);
}

void CollectorRobot::isReady()
{
	ready = true;
}


int CollectorRobot::moveTo(string dest)
{
	int time = path_times[currentPoint + "to" + dest];

	for (int i = 0; i < time; ++i) {
		sendCommand(dest.at(0));
		printMap(dest);
		Sleep(S_TIME);
	}
	currentPoint = dest;
	printMap(dest);
	sendCommand(MOVE_COMPLETE);
	return time;

}

void CollectorRobot::printMap(string dest) {
	static bool blink = false;
	string top = TL_CORNER + H_STRAIGHT + T_TCONNECTOR + H_STRAIGHT + TR_CORNER;
	string middle1 = V_STRAIGHT + EMPTY + V_STRAIGHT + EMPTY + V_STRAIGHT;
	string middle2 = L_TCONNECTOR + H_STRAIGHT + PCONNECTOR + H_STRAIGHT + R_TCONNECTOR;
	string bottom = BL_CORNER + H_STRAIGHT + B_TCONNECTOR + H_STRAIGHT + BR_CORNER;
	int nline = MAP_OFFSET + 1;
	
	printer->printMap(COLLECTOR_WINDOW, nline, MAP_STARTCOL, top);
	for (int i = 1; i < warehouseIDs.size()+1; i++) {
		string whIcon = warehouseIDs[i - 1] + ' ';
		if (whIcon.size() == 2) { whIcon = ' ' + whIcon; }
		if (currentPoint == warehouseIDs[i - 1] && currentPoint == dest) {
			middle1 = V_STRAIGHT + whIcon + V_STRAIGHT + COLLECTOR + V_STRAIGHT;
		}
		else if (dest == warehouseIDs[i - 1] && currentPoint != dest) {
			if (blink) { 
				blink = false;
				middle1 = V_STRAIGHT + whIcon + V_STRAIGHT + EMPTY + V_STRAIGHT;
			}
			else {
				blink = true;
				middle1 = V_STRAIGHT + whIcon + V_STRAIGHT + COLLECTOR + V_STRAIGHT;
			}
		}
		else {
			middle1 = V_STRAIGHT + whIcon + V_STRAIGHT + EMPTY + V_STRAIGHT;
		}
		
		printer->printMap(COLLECTOR_WINDOW, nline+(i * MAPSPACE_HEIGHT) - 1, MAP_STARTCOL, middle1);
		if (i < warehouseIDs.size()) {
			printer->printMap(COLLECTOR_WINDOW, nline + (i * MAPSPACE_HEIGHT), MAP_STARTCOL, middle2);
		}
	}
	printer->printMap(COLLECTOR_WINDOW, nline + warehouseIDs.size() * 2, MAP_STARTCOL, bottom);

	printer->printString(COLLECTOR_WINDOW, MOVE_NLINE, MOVE_NCOL, "Time = " + to_string(totalTime) + " sec");
	printer->printString(COLLECTOR_WINDOW, BASKET_NLINE, BASKET_NCOL,"Basket: " + to_string(nrItemsInBasket) + "/" + to_string(basketSize));

	printer->refreshw(COLLECTOR_WINDOW);
}

Order CollectorRobot::getOrder(string warehouseID)
{
	lock_guard<mutex> lock(order_mutex);
	Order o = { 0, "", 0, 0, "", 0, 0, "" };
	if (ordersReady.find(warehouseID) != ordersReady.end()) {
		o = ordersReady[warehouseID].back();
		ordersReady[warehouseID].pop_back();
		return o;
	}
	return o;
}

void CollectorRobot::collectOrder(string warehouseID)
{
	if (ordersReady[warehouseID].size() > 0) {
		if (currentPoint != warehouseID) { totalTime += moveTo(warehouseID); }
		while (nrItemsInBasket < basketSize && ordersReady[warehouseID].size() > 0) {
			Order o = getOrder(warehouseID);	
			ordersInBasket.push_back(o);
			nrItemsInBasket++;
			printMap(currentPoint);
			Sleep(S_TIME);
			totalTime += 1;
		}
		unload();
	}
}

bool CollectorRobot::sendCommand(const char c)
{
	if (serial.IsOpened()) {
		char message[5] = "F";
		assert(serial.SendData(c));

		if (serial.ReadDataWaiting() > 0) {
			serial.ReadData(message, 1);
		}
	}
	else {
		//cout << "Serial port is not open!!! \n";
	}
	return true;
}

int CollectorRobot::unload()
{
	totalTime += moveTo(LOADINGDOCK);		// move to loading dock
	while (nrItemsInBasket > 0) {
		Order order = ordersInBasket.back();
		loadingDock->addOrdersforTruck(order);
		nrItemsInBasket--;
		ordersInBasket.pop_back();
		printMap(LOADINGDOCK);
		Sleep(S_TIME);
		totalTime += 1;
	}

	return 0;
}
