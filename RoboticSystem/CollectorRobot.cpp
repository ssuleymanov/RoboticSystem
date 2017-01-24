#include "CollectorRobot.h"

CollectorRobot::CollectorRobot()
{
}

CollectorRobot::~CollectorRobot()
{
}

CollectorRobot::CollectorRobot(int basketsize, LoadingDock& ld, string filename)
{
	this->basketSize = basketsize;
	this->loadingDock = &ld;
	this->currentPoint = "LD";
	ready = false;
	nrItemsInBasket = 0;
	totalTime = 0;
	warehouseIDs.push_back("LD");
	wh_ready = false;

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

//CollectorRobot::CollectorRobot(const CollectorRobot & collector) :
//	baudRate(collector.baudRate),
//	portNumber(collector.portNumber),
//	basketSize(collector.basketSize),
//	nrItemsInBasket(collector.nrItemsInBasket),
//	totalTime(collector.totalTime),
//	warehouseIDs(collector.warehouseIDs),
//	ready(collector.ready),
//	currentPoint(collector.currentPoint),
//	ordersReady(collector.ordersReady),
//	ordersInBasket(collector.ordersInBasket),
//	path_times(collector.path_times),
//	serial(collector.serial),
//	loadingDock(collector.loadingDock)
//{
//}

void CollectorRobot::setupSerial(int baudrate, int portnumber)
{
	this->baudRate = baudrate;
	this->portNumber = portnumber;
}

void CollectorRobot::startRobot(Printer* printr)
{
	printer = printr;
	startTime = clock();
	printMap(currentPoint);
	if (serial.Open(portNumber, baudRate)) {
		printer->printLog(LOG_INFO,"X", "Port " + to_string(portNumber) + " opened succesfully..");
	}
	else {
		printer->printLog(LOG_ERROR, "X", "Failed to open port " + to_string(portNumber) + " ..!");
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
			else if (bestSize == 0) { unload(); }

			if (bestSize == 0 && ready) { break; }
		}
		Sleep(100);
		printMap(currentPoint);
	}
	printer->printString("collector", MOVE_NLINE, MOVE_NCOL, "Time = " + to_string(totalTime) + " sec");
}

void CollectorRobot::addOrder(Order order)
{
	lock_guard<mutex> lock(order_mutex);
	ordersReady[order.warehouseID].push_back(order);
}

int CollectorRobot::moveTo(string dest)
{
	int time = path_times[currentPoint + "to" + dest];

	for (int i = 0; i < time; ++i) {
		if (i == time) { currentPoint = dest; }
		printMap(dest);
		Sleep(S_TIME);
	}
	currentPoint = dest;
	return time;

}

void CollectorRobot::printMap(string dest) {
	static bool blink = false;
	string top = string(1, 201) + string(3, 205) + string(1, 203) + string(3, 205) + string(1, 187);
	string middle1 = string(1, 186) + "   " + string(1, 186) + "   " + string(1, 186);
	string middle2 = string(1, 204) + string(3, 205) + string(1, 206) + string(3, 205) + string(1, 185);
	string bottom = string(1, 200) + string(3, 205) + string(1, 202) + string(3, 205) + string(1, 188);
	int nline = MAP_OFFSET+2;
	
	printer->printMap("collector", nline, 3, top);
	for (int i = 1; i < warehouseIDs.size()+1; i++) {
		string whIcon = warehouseIDs[i - 1] + ' ';
		if (whIcon.size() == 2) { whIcon = ' ' + whIcon; }
		if (currentPoint == warehouseIDs[i - 1] && currentPoint == dest) {
			middle1 = string(1, 186) + whIcon + string(1, 186) + " X " + string(1, 186);
		}
		else if (dest == warehouseIDs[i - 1] && currentPoint != dest) {
			if (blink) {
				blink = false;
				middle1 = string(1, 186) + whIcon + string(1, 186) + "   " + string(1, 186);
			}
			else {
				blink = true;
				middle1 = string(1, 186) + whIcon + string(1, 186) + " X " + string(1, 186);
			}
		}
		else {
			middle1 = string(1, 186) + whIcon + string(1, 186) + "   " + string(1, 186);
		}
		
		printer->printMap("collector", nline+(i*2)-1, 3, middle1);
		if (i < warehouseIDs.size()) {
			printer->printMap("collector", nline + (i * 2), 3, middle2);
		}
	}
	totTime = (clock() - startTime)/ CLK_TCK;
	printer->printString("collector", ACTION_NLINE, ACTION_NCOL, "real time = " + to_string(totTime) + " sec");
	printer->printString("collector", MOVE_NLINE, MOVE_NCOL, "Time = " + to_string(totalTime) + " sec");
	printer->printMap("collector", nline + warehouseIDs.size()*2, 3, bottom);
	printer->printString("collector", BASKET_NLINE, BASKET_NCOL,"Basket: " + to_string(nrItemsInBasket) + "/" + to_string(basketSize));


	printer->refreshw("collector");
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
	
		if (nrItemsInBasket == basketSize) { unload(); }
	}
}

bool CollectorRobot::sendCommand(const char c)
{

	return true;
}

int CollectorRobot::unload()
{
	totalTime += moveTo("LD");		// move to loading dock
	while (nrItemsInBasket > 0) {
		Order order = ordersInBasket.back();
		loadingDock->sortOrderbyPriority(order);
		nrItemsInBasket --;
		ordersInBasket.pop_back();
		printMap("LD");
		Sleep(S_TIME);
		totalTime += 1;
	}

	return 0;
}

int CollectorRobot::getNrItemsInBasket()
{
	return nrItemsInBasket;
}

void CollectorRobot::isReady()
{
	ready = true;
}

string CollectorRobot::getCurrentPoint()
{
	return currentPoint;
}

void CollectorRobot::warehouseReady()
{
	wh_ready = true;
}

void CollectorRobot::addWarehouseID(string warehouseID)
{
	warehouseIDs.push_back(warehouseID);
}
