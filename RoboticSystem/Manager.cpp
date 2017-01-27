#include "Manager.h"

Manager::Manager() : collector(loadingDock, "path_times.txt")
{
	warehouses.empty();
	printer = Printer::getInstance();
	menuOn = true;
}

Manager::~Manager()
{
}

void Manager::setup(string config_file, string article_file)
{
	readArticles(article_file);

	// read collector config file
	ifstream file1(COLLECTOR_CONFIG_FILE);
	int basket, port, baud;
	string cvalue;
	getline(file1, cvalue);	// skip the header
	getline(file1, cvalue);
	stringstream ss(cvalue);
	ss >> basket >> port >> baud;
	collector.InitCollector(basket, baud, port);

	ifstream file(config_file);
	string value;
	getline(file, value); //Skip Header
	while (getline(file, value)) {
		char whID[50];
		int rows, cols, unload_x, unload_y, start_x, start_y, size;
		size = sscanf_s(value.c_str(), "%s\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d\t%d", &whID, 50, &rows, &cols, &start_x, &start_y, &unload_x, &unload_y, &basket, &port, &baud);
		if (size == WAREHOUSE_CONFIG_SIZE)
		{
			addWarehouse(Warehouse(whID, rows, cols));
			PickerRobot pRobot(basket, *this);
			pRobot.setSerialParameters(port, baud);
			addRobotController(RobotController(pRobot, getWarehouse(whID)));
			for (auto& controller : rControllers) { if (whID == controller.getWarehouseID()) { controller.Initialize(Point(start_x, start_y), Point(unload_x, unload_y), printer); } }
		}
		else {
			printer->printLog(LOG_ERROR, "M", "Expecting 10 arguments in the warehouse configuration, recieved " + to_string(size));
		}
	}
	//setupDisplay();
}

void Manager::setupDisplay()
{
	int mapOffset = 0;
	system("cls");
	resize_term(MAX_SCREEN_HEIGHT, MAX_WINDOW_WIDTH);
	for (RobotController rController : rControllers) {
		Warehouse wh = getWarehouse(rController.getWarehouseID());
		collector.addWarehouseID(wh.getWarehouseID());
		mapOffset += printer->addWindow(wh, mapOffset);
		resize_term(MAX_SCREEN_HEIGHT, mapOffset + MAX_WINDOW_WIDTH);
	}
	mapOffset += printer->addWindow("collector", MAX_WINDOW_HEIGHT, COLLECTOR_WINDOW_WIDTH, mapOffset, VMAP_OFFSET);
	resize_term(MAX_SCREEN_HEIGHT, mapOffset + MAX_WINDOW_WIDTH);
	mapOffset += printer->addWindow("log", MAX_WINDOW_HEIGHT, LOG_WINDOW_WIDTH, mapOffset, VMAP_OFFSET);
	resize_term(MAX_SCREEN_HEIGHT, mapOffset);
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

	thread controlThread (&Manager::ControlPanel, this, 30);
	for (int i = 0; i < rControllers.size(); i++) {
		threads.push_back(thread(&RobotController::startAutomaticRobot, &rControllers[i]));
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
}

bool Manager::manualControl(string productID, int quantity)
{
	printer->clearWindows();
	printer->drawBoxes();
	Article article = articles[productID];
	vector<Order> manOrder;
	Order order = { article.compartment, "", 0, 0, productID, quantity, 0, article.warehouseID };
	manOrder.push_back(order);

	for (vector<RobotController>::iterator i = this->rControllers.begin(), end = rControllers.end(); i != end; i++) {

		if (i->getWarehouseID() == order.warehouseID) {
			i->startManualRobot(manOrder);
		}
	}
	return true;
}

bool Manager::productValid(string productID)
{
	return (articles.find(productID) != articles.end());
}

void Manager::orderIsDone(vector<Order> orders)
{
	lock_guard<mutex> guard(collect_mutex);
	for (auto& order : orders) {
		collector.addOrder(order);
	}
}

void Manager::orderIsInvalid(string customerID)
{
	loadingDock.InvalidOrder(customerID);
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
	list<Warehouse>::iterator it;
	it = find_if(warehouses.begin(), warehouses.end(), [=](Warehouse wh) {return (wh.getWarehouseID() == WarehouseID); });
	if (it != warehouses.end()) { return *it; }

	printer->printLog(LOG_ERROR, "M", "Warehouse " + WarehouseID + "does not exist");

	stringstream sstm;
	sstm << "Warehouse " << WarehouseID << " does not exist";
	throw std::runtime_error(sstm.str());
}


RobotController& Manager::getRobotController(string WarehouseID)
{
	vector<RobotController>::iterator it;
	it = find_if(rControllers.begin(), rControllers.end(), [=](RobotController rC) {return (rC.getWarehouseID() == WarehouseID); });
	if (it != rControllers.end()) { return *it; }

	printer->printLog(LOG_ERROR, "M", "PickerRobot for Warehouse " + WarehouseID + "does not exist");

	stringstream sstm;
	sstm << "PickerRobot for Warehouse " << WarehouseID << " does not exist";
	throw std::runtime_error(sstm.str());
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
			}
			else {
				string pID = productID;
				printer->printLog(LOG_ERROR,"M","Product with ID: " + pID + " does not exist");
			}
		}
		else {
			printer->printLog(LOG_ERROR, "M", "Incorrect or missing parameters!!! Check the OPL line: " + to_string(priority + 1));
		}
		priority++;
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

void Manager::ControlPanel(int offset) {
	char operation = ' ';
	char warehouseID = ' ';
	bool wh_correct = false;
	WINDOW* win = newwin(1, offset, 0, 0);
	while (menuOn) {
		warehouseID = wgetch(win);
		if (!menuOn) { return; }
		operation = wgetch(win);
		if (!menuOn) { return; }
		for (auto &rController : rControllers) {
			if (warehouseID == rController.getWarehouseID().at(0) || warehouseID == (char)tolower(rController.getWarehouseID().at(0))) {
				if (tolower(operation) == 's') {
					rController.getPickerRobot().emergency_stop(true);
				}
				else if (tolower(operation) == 'p') {
					rController.getPickerRobot().pauseRobot(true);
				}
				else if (tolower(operation) == 'r') {
					rController.getPickerRobot().emergency_stop(false);
					rController.getPickerRobot().pauseRobot(false);
				}
				else {
					wclear(win);
					wprintw(win, "Wrong OPERATION");
				}

				wh_correct = true;
				break;
			}
		}

		if (wh_correct) {
			wh_correct = false;
		}
		else {
			wclear(win);
			wprintw(win, "Wrong WAREHOUSE");
		}

		wgetch(win);
		wclear(win);
	}
}