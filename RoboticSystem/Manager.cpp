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
			PickerRobot pRobot(basket);
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
}

void Manager::execute(string oplFile, string aticleFile)
{
	vector<Order> orderList;
	vector<thread> threads;
	orderList = readOPL(oplFile, aticleFile);
	
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


	for (int j = 0; j < rControllers.size(); j++) {
		if (threads[j].joinable()) {
			threads[j].join();
		}
	}
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

vector<Order> Manager::readOPL(string oplFile, string articleFile)
{
	ifstream articleStream(articleFile);
	string value;
	map<string, Article> ArticleList;

	getline(articleStream, value); //Skip Header
	while (getline(articleStream, value)) {

		char productID[20], warehouseID[20];
		int compartment, size;
		size = sscanf_s(value.c_str(), " %[^;];%[^;];%d", &productID, 20, &warehouseID, 20, &compartment);
		if (size == 3)
		{
			Article article = { productID, warehouseID, compartment };
			ArticleList.insert(pair<string, Article>(productID, article));
			cout << "Added article: " << article.productID << endl;
		}
		else {
			cout << "[Error] Article should contain 3 arguments, recieved " << size << endl;
		}

	}


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
			Article article = ArticleList[productID];
			Order order = { article.compartment, customerID, orderID, priority, productID, quantity, truckNr, article.warehouseID };
			cout << "Added Order: " << orderID << endl;
			orderList.push_back(order);
			priority++;
		}
		else {
			cout << "[Error] Order should contain 5 arguments, recieved " << size << endl;
		}
	}
	return orderList;
}
