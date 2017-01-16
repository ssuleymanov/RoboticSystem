#include "Manager.h"

Manager::Manager()
{
	warehouses.empty();
	printer = Printer::getInstance();
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
			RobotController rController(pRobot, getWarehouse(whID));
			rController.setStartingPoint(Point(start_x,start_y));
			rController.setUnloadingPoint(Point(unload_x,unload_y));
			addRobotController(rController);
		}
		else {
			cout << "[Error] expecting 10 arguments in the warehouse configuration, recieved " << size << endl;
		}
	}
	loadingDock = LoadingDock();
	collector = CollectorRobot(16,loadingDock,"path_times.txt");
	collector.setupSerial(9600,4);
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
	int mapOffset = 0;
	system("cls");
	for (RobotController rController : rControllers) {
		Warehouse wh = getWarehouse(rController.getWarehouseID());
		mapOffset += printer->addWindow(wh,mapOffset);
	}

	for (int i = 0; i < rControllers.size(); i++) {
		threads.push_back(thread(&RobotController::startRobot, &rControllers[i], printer));
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
	system("cls");
	loadingDock.printOrders();
}

void Manager::manualControl(string fileName)
{
	string productID, WarehouseID;
	int compartment, quantity;
	ifstream file(fileName);
	string value;
	Order order;

	getline(file, value); //Skip Header
	while (getline(file, value)) {
		stringstream ss(value);
		ss >> productID >> WarehouseID >> compartment >> quantity;

		order.productID = productID;
		order.warehouseID = WarehouseID;
		order.compartment = compartment;
		order.quantity = quantity;

		for (vector<RobotController>::iterator i = this->rControllers.begin(), end = rControllers.end(); i != end; i++) {
			if (i->getWarehouseID() == order.warehouseID) {
				i->getOrder(order);
			}
		}
	}
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
			Article article = articles[productID];
			if (article.warehouseID != "") {
				Order order = { article.compartment, customerID, orderID, priority, productID, quantity, truckNr, article.warehouseID };
				clog << "Added Order: " << orderID << endl;
				orderList.push_back(order);
				priority++;
			}
			else {
				cerr << "Product with ID: " << productID << " does not exist";
			}
		}
		else {
			cerr << "[Error] Order should contain 5 arguments, recieved " << size << endl;
		}
	}
	return orderList;
}

void Manager::readArticles(string articleFile) {

	file<> f(articleFile.c_str());

	xml_document<> doc;
	doc.parse<0>(f.data());

	xml_node<> *pRoot = doc.first_node("Workbook");
	for (xml_node<> *node = pRoot->first_node("Worksheet");
		node; node = node->next_sibling())
	{
		xml_attribute<> *wname = node->first_attribute();
		char warehouseID[20], temp[30];
		vector<string> data;
		string attribute = wname->value();
		sscanf_s(attribute.c_str(), " %[^(](%[^)])", &temp, 30, &warehouseID, 20);
		cout << "Node Worksheet has value " << warehouseID << "\n";

		xml_node<> *node2 = node->first_node("Table");
		int i = 0;
		Article product;
		for (xml_node<> *node3 = node2->first_node("Row");
			node3; node3 = node3->next_sibling())
		{
			xml_attribute<> *attr2 = node3->first_attribute();
			xml_node<> *snode = node3->next_sibling();
			if (attr2 != NULL || snode == NULL) {
				if (i < 3) {
					product.compartment = 0;
				}
				i = 0;
				product.warehouseID = warehouseID[2];
				articles.insert(pair<string, Article>(product.productID, product));
			}

			for (xml_node<> *node4 = node3->first_node("Cell");
				node4; node4 = node4->next_sibling())
			{
				xml_node<> *node5 = node4->first_node("Data");
				xml_node<> *node6 = node5->first_node();
				if (i == 1) {
					product.productID = node6->value();
				}
				else if (i == 3) {
					string comp = node6->value();
					product.compartment = stoi(comp);
					
				}
				i++;
				//data.push_back(node6->value());
				//cout << "Node Data has value: " << node6->value() << "\n";
			}
			//i++;
		}
		/*int i = 0;
		Article product;
		for (auto& cell : data) {
			if (i == 1) {
				product.productID = cell;
				i++;
			}
			else if (i == 3) {
				product.compartment = stoi(cell);
				product.warehouseID = warehouseID;
				articles.insert(pair<string, Article>(product.productID, product));
				i = 0;
			}
			else {
				i++;
			}
		}*/
	}
	//string value;
	//cout << "Article List made.\n";

	//getline(articleStream, value); //Skip Header
	//while (getline(articleStream, value)) {

	//	char productID[20], warehouseID[20];
	//	int compartment, size;
	//	size = sscanf_s(value.c_str(), " %[^;];%[^;];%d", &productID, 20, &warehouseID, 20, &compartment);
	//	if (size == 3)
	//	{
	//		Article article = { productID, warehouseID, compartment };
	//		articles.insert(pair<string, Article>(productID, article));
	//		cout << "Added article: " << article.productID << endl;
	//	}
	//	else {
	//		cout << "[Error] Article should contain 3 arguments, recieved " << size << endl;
	//	}
	//}
}