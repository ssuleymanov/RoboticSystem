#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <ctime>
#include <vector>
#include <sstream>
#include <stdio.h>

#include "Serial.h"
#include "Point.h"
#include "Warehouse.h"
#include "PickerRobot.h"
#include "RobotController.h"
#include "Manager.h"
#include "CollectorRobot.h"
#include "LoadingDock.h"
#include <pdcwin.h>

#define TEST false

using namespace std;

string current_time() {
	time_t rawtime;
	struct tm timeinfo;
	char buffer[80];

	time(&rawtime);
	localtime_s(&timeinfo, &rawtime);

	strftime(buffer, 80, "%d-%m-%Y %I:%M:%S", &timeinfo);
	string str(buffer);

	return str;
}


void write_header() {
	clog << "Starting: " << current_time() << endl << endl;
	cerr << "Starting: " << current_time() << endl << endl;
}

void write_footer() {
	clog << "Ending: " << current_time() << endl << endl;
	cerr << "Ending: " << current_time() << endl << endl;
}

int main() {
	
	//system("exec rm -r /Logs/*");
	system("del /Q Logs\\*.txt");
	filebuf os;
	os.open("Logs\\log.txt", fstream::app);
	streambuf* oldbuf = clog.rdbuf(&os);

	filebuf oss;
	oss.open("Logs\\err.txt", fstream::app);
	streambuf* oldbuff = cerr.rdbuf(&oss);

	write_header();



#if TEST
	// moveTo Unit Test
	Point start_point(1, 1);
	Point unload_point(5, 5);

	Warehouse whouse("C", 5, 6);
	PickerRobot robot(5);
	robot.setSerialParameters(3, 9600);
	Printer* printer = Printer::getInstance();

	RobotController r_controller(robot, whouse);
	r_controller.setStartingPoint(start_point);
	r_controller.setUnloadingPoint(unload_point);
	r_controller.startRobot(printer);

	Order order, order2, order3, order4, order5, order6;
	order.quantity = 1;
	order.compartment = 11;
	order.customerID = "1254";
	order.productID = "1111";
	order.priority = 1;
	order.truckNr = 2;

	order2.quantity = 2;
	order2.compartment = 17;
	order2.customerID = "1254";
	order2.productID = "1112";
	order2.priority = 2;
	order2.truckNr = 1;

	order3.quantity = 3;
	order3.compartment = 29;
	order3.customerID = "1254";
	order3.productID = "1113";
	order3.priority = 3;
	order3.truckNr = 3;

	order4.quantity = 2;
	order4.compartment = 3;
	order4.customerID = "1254";
	order4.productID = "1114";
	order4.priority = 4;
	order4.truckNr = 1;

	order5.quantity = 1;
	order5.compartment = 20;
	order5.customerID = "1254";
	order5.productID = "1115";
	order5.priority = 5;
	order5.truckNr = 1;

	order6.quantity = 7;
	order6.compartment = 5;
	order6.customerID = "1254";
	order6.productID = "1116";
	order6.priority = 6;
	order6.truckNr = 1;

	vector<Order> orders;
	orders.push_back(order);
	orders.push_back(order2);
	orders.push_back(order3);
	orders.push_back(order4);
	orders.push_back(order5);
	orders.push_back(order6);

	whouse.setUnloadedOrders(orders);
	LoadingDock ldock;
	CollectorRobot cr(16, ldock, "path_times.txt");
	cr.loadOrders(whouse);
	cr.unload();

	for (auto& a : ldock.getOrdersperTruck(1))  cout << "product id tr1: " << a.productID << endl;
	for (auto& a : ldock.getOrdersperTruck(2))  cout << "product id tr2: " << a.productID << endl;
	for (auto& a : ldock.getOrdersperTruck(3))  cout << "product id tr3: " << a.productID << endl;
	for (auto& a : whouse.getUnloadedOrders())  cout << "product id warehouse: " << a.productID << endl;

	//r_controller.executeOrders(orders);
	//r_controller.getOrder(order6);
	
#else

	Manager manager;
	string fileName;
	string parameter;
	char param1;
	int i = 1;
	

	cout << "(A)utomatic or (M)anual?";
	param1 = getchar();

	ifstream input_file("input.txt");
	string fileParams[3];

	for (int i = 0; i < 3; i++) {
		getline(input_file, parameter);
		fileParams[i] = parameter;
	}

	/*for (int i = 0; i < 4; i++) {
		cout << "File parameters: " << fileParams[i] << ", ";
	}
	cout << endl;*/

	initscr();
	start_color();
	//init_color(COLOR_BLUE,400,400,400);
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	cbreak();
	//noecho();
	curs_set(0);

	if (param1 == 'A' || param1 == 'a') {
		noecho();
		manager.setup(fileParams[0]); //"wh_config.txt"
		manager.readArticles(fileParams[1]);//"Article_List_XML.xml"
		manager.execute(fileParams[2]);//"Order_Picking_List.csv"
	}
	else if (param1 == 'M' || param1 == 'm') {
		manager.readArticles(fileParams[1]);
		char productID[20], quantity[5];
		printw("ProductID:  ") ;
		while (true) {
			getstr(productID);
			if (manager.productValid(productID)) {
				break;
			}
			else if (productID[0] == '\0') { return (EXIT_FAILURE); }
			else { 
				clear();
				printw("This Product does not exist, please try again or press enter to stop\n");
				printw("ProductID:  ");
			}
		}
		clear();
		printw("Quantity:  ");
		int q = 0;
		while (true) {
			getstr(quantity);
			try {
				q = stoi(quantity);
				if (q == 0) {
					clear();
					printw("Quantity cannot be 0, please try again or press enter to stop\n");
					printw("Quantity:  ");
				}
				else { break; }
				
			}
			catch (exception e) {
				clear();
				printw("%s is not a number, please try again or press enter to stop\n",quantity);
				printw("Quantity:  ");
			}
			if (quantity[0] == '\0') { return (EXIT_FAILURE); }
		}
		noecho();
		manager.setup(fileParams[0]); //
		manager.manualControl(productID,q);//"manual_orders.txt"
		getchar();
	}
	else {
		cout << "Incorrect \n";
		getchar();
	}

#endif


	write_footer();
	clog.rdbuf(oldbuf);
	cerr.rdbuf(oldbuff);


	return 0;

}
