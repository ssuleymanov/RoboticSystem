#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <ctime>
#include <vector>

#include "Serial.h"
#include "Point.h"
#include "Warehouse.h"
#include "PickerRobot.h"
#include "RobotController.h"
#include "Order.h"

#define TEST true

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
}

void write_footer() {
	clog << "Ending: " << current_time() << endl << endl;
}

int main(void) {
	
	filebuf os;
	os.open("log.txt", fstream::app);
	streambuf* oldbuf = clog.rdbuf(&os);

	write_header();

#if TEST
	// moveTo Unit Test
	Point start_point(1, 1);
	Point unload_point(5, 5);

	Warehouse whouse("C", 5, 6);
	PickerRobot robot(5);
	robot.setSerialParameters(3, 9600);

	RobotController r_controller(robot, whouse);
	r_controller.setStartingPoint(start_point);
	r_controller.setUnloadingPoint(unload_point);
	r_controller.startRobot();

	Order order, order2, order3, order4, order5, order6;
	order.amountOfItems = 1;
	order.compartmentPosition = 11;
	order.customerID = "1254";
	order.productID = "1111";
	order.priority = 1;
	order.truckNumber = 2;

	order2.amountOfItems = 2;
	order2.compartmentPosition = 17;
	order2.customerID = "1254";
	order2.productID = "1112";
	order2.priority = 2;
	order2.truckNumber = 1;

	order3.amountOfItems = 3;
	order3.compartmentPosition = 29;
	order3.customerID = "1254";
	order3.productID = "1113";
	order3.priority = 3;
	order3.truckNumber = 3;

	order4.amountOfItems = 2;
	order4.compartmentPosition = 3;
	order4.customerID = "1254";
	order4.productID = "1114";
	order4.priority = 4;
	order4.truckNumber = 1;

	order5.amountOfItems = 1;
	order5.compartmentPosition = 20;
	order5.customerID = "1254";
	order5.productID = "1115";
	order5.priority = 5;
	order5.truckNumber = 1;

	order6.amountOfItems = 7;
	order6.compartmentPosition = 5;
	order6.customerID = "1254";
	order6.productID = "1115";
	order6.priority = 6;
	order6.truckNumber = 1;

	vector<Order> orders;
	orders.push_back(order);
	orders.push_back(order2);
	orders.push_back(order3);
	orders.push_back(order4);
	orders.push_back(order5);


	r_controller.executeOrders(orders);
	r_controller.getOrder(order6);
	
#else

	Controller manager;
	manager.setup("wh_config.txt");
	manager.execute("test.csv");

#endif


	write_footer();
	clog.rdbuf(oldbuf);

	return 0;

}
