#include "Manager.h"

void Manager::ControlPanel(int offset) {
	char c;
	WINDOW* win = newwin(1, offset, 0, 0);
	while (menuOn) {
		c = ' ';
		c = wgetch(win);
		if (c == 'x') {
			mvwaddstr(win, 0, 1, "Stopping");
		}
		else {
			mvwaddch(win, 0, 1, c);
		}
	}
}

Manager::Manager()
{
	warehouses.empty();
	printer = Printer::getInstance();
	menuOn = true;
}

Manager::~Manager()
{
}

void Manager::setup(string fileName)
{

	//Read Warehouse settings
	ifstream file(fileName);
	string value;
	getline(file, value); //Skip Header
	while (getline(file, value)) {
		char whID[50];
		int rows, cols, unload_x, unload_y, start_x, start_y, size;
		int basket, port, baud;
		size = sscanf_s(value.c_str(), "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", &whID, 50, &rows, &cols, &start_x, &start_y, &unload_x, &unload_y, &basket, &port, &baud);
		if (size == 10)
		{
			addWarehouse(Warehouse(whID, rows, cols));
			PickerRobot pRobot(basket, *this);
			pRobot.setSerialParameters(port, baud);
			addRobotController(RobotController(pRobot, getWarehouse(whID)));
			for (auto& controller : rControllers) { if (whID == controller.getWarehouseID()) {controller.Initialize(Point(start_x, start_y), Point(unload_x, unload_y), printer); } }
		}
		else {
			printer->printLog(LOG_ERROR,"M","Expecting 10 arguments in the warehouse configuration, recieved " + to_string(size));
		}
	}
	loadingDock = LoadingDock();
	collector = CollectorRobot(16,loadingDock,"path_times.txt");
	collector.setupSerial(9600,4);

	int mapOffset = 0;
	system("cls");
	resize_term(60, 50);
	for (RobotController rController : rControllers) {
		Warehouse wh = getWarehouse(rController.getWarehouseID());
		mapOffset += printer->addWindow(wh, mapOffset);
		resize_term(60, mapOffset + 50);
	}
	mapOffset += printer->addWindow("log", 50, 45, mapOffset, 2);
	resize_term(60, mapOffset);
	printer->drawBoxes();
}

void Manager::execute(string oplFile)
{
	vector<Order> orderList;
	vector<thread> threads;
	thread collectThread;
	orderList = readOPL(oplFile);
	
	for (Order order : orderList) {
		getWarehouse(order.warehouseID).addOrder(order);
	}

	thread controlThread(&Manager::ControlPanel,this,20);
	for (int i = 0; i < rControllers.size(); i++) {
		threads.push_back(thread(&RobotController::startRobot, &rControllers[i]));
	}

	collectThread = thread(&CollectorRobot::startRobot, &collector, printer);

	for (int j = 0; j < threads.size(); j++) {
		if (threads[j].joinable()) {
			threads[j].join();
		}
	}
	
	collector.isReady();
	if (collectThread.joinable()) {
		collectThread.join();	
	}
	menuOn = false;
	controlThread.join();
	loadingDock.printOrders(printer);
	getchar();
}

bool Manager::manualControl(string productID, int quantity)
{
	Article article = articles[productID];
	Order order = { article.compartment, "", 0, 0, productID, quantity, 0, article.warehouseID };
	if (articles.find(productID) == articles.end()) {
		cout << "Product with ID: " << productID << " does not exist";
		return false;
	}

	for (vector<RobotController>::iterator i = this->rControllers.begin(), end = rControllers.end(); i != end; i++) {
		if (i->getWarehouseID() == order.warehouseID) {
			Warehouse wh = i->getWarehouseID();
			printer->addWindow(wh, 0);
			i->startManualRobot(order);
		}
	}
	return true;
}

bool Manager::productValid(string productID)
{
	return (articles.find(productID) != articles.end());
}



void Manager::orderIsDone(Order order)
{
	lock_guard<mutex> guard(collect_mutex);
	collector.addOrder(order);
}

void Manager::addWarehouse(Warehouse wh)
{
	warehouses.push_back(wh);
}

void Manager::addRobotController(RobotController rController)
{
	rControllers.push_back(rController);
}

Warehouse & Manager::getWarehouse(string WarehouseID)
{
	for (list<Warehouse>::iterator i = this->warehouses.begin(), end = warehouses.end(); i != end; i++) {
		if (i->getWarehouseID() == WarehouseID) {
			return *i;
		}
	}
	string err_msg;
	stringstream sstm;
	sstm << "Warehouse " << WarehouseID << " does not exist";
	err_msg = sstm.str();
	throw std::runtime_error(err_msg);
}


RobotController& Manager::getRobotController(string WarehouseID)
{
	for (vector<RobotController>::iterator i = this->rControllers.begin(), end = rControllers.end(); i != end; i++) {
		if (i->getWarehouseID() == WarehouseID) {
			return *i;
		}
	}
	string err_msg;
	stringstream sstm;
	sstm << "PickerRobot for Warehouse " << WarehouseID << " does not exist";
	err_msg = sstm.str();
	throw std::runtime_error(err_msg);
}

vector<Order> Manager::readOPL(string oplFile)
{
	string value;
	ifstream oplStream(oplFile);
	vector<Order> orderList;
	int priority = 1;

	getline(oplStream, value); //Skip Header

	while (getline(oplStream, value)) {
		char customerID[20], productID[20];
		int orderID, truckNr, quantity, size;
		size = sscanf_s(value.c_str(), " %d;%[^;];%d;%[^;];%d", &orderID, &customerID, 20, &truckNr, &productID, 20, &quantity);
		if (size == 5)
		{		
			if (articles.find(productID) != articles.end()) {
				Article article = articles[productID];
				Order order = { article.compartment, customerID, orderID, priority, productID, quantity, truckNr, article.warehouseID };
				printer->printLog(LOG_INFO,"M","Added Order: " + to_string(orderID) + " - " + productID);
				orderList.push_back(order);
				priority++;
			}
			else {
				string pID = productID;
				printer->printLog(LOG_ERROR,"M","Product with ID: " + pID + " does not exist");
			}
		}
		else {
			printer->printLog(LOG_ERROR,"M","Order should contain 5 arguments, recieved " + to_string(size));
		}
	}
	return orderList;
}

void Manager::readArticles(string articleFile) {

	file<> f(articleFile.c_str());

	xml_document<> doc;
	doc.parse<0>(f.data());

	xml_node<> *pRoot = doc.first_node("Workbook");
	for (xml_node<> *node = pRoot->first_node("Worksheet"); // Every page is one worksheet
		node; node = node->next_sibling())
	{													
		char warehouseID[20], temp[30];
		vector<string> data;
		string attribute = node->first_attribute()->value();
		sscanf_s(attribute.c_str(), " %[^(](%[^)])", &temp, 30, &warehouseID, 20);

		int i = 0;
		Article product;
		for (xml_node<> *node3 = node->first_node("Table")->first_node("Row");
			node3; node3 = node3->next_sibling())
		{
			for (xml_node<> *node4 = node3->first_node("Cell");
				node4; node4 = node4->next_sibling())
			{
				xml_node<> *node6 = node4->first_node("Data")->first_node();
				if (i == 1) { //Second cell contains the productID
					product.productID = node6->value();
				}
				else if (i == 3) { //Fourth cell contains the compartment number
					string comp = node6->value();
					product.compartment = stoi(comp);
				}
				i++;
			}
			xml_node<> *snode = node3->next_sibling();
			if (snode == NULL || snode->first_attribute() != NULL) { //If this is the last row save the article
				if (i == 4) { // If 4 cells were found save as article
					product.warehouseID = warehouseID[2];
					articles.insert(pair<string, Article>(product.productID, product));
				}
				i = 0;				
			}
		}
	}
}