#include <iostream>
#include <fstream>
#include <string>
#include <assert.h>
#include <ctime>

#include "Serial.h"
#include "Point.h"
#include "Warehouse.h"
#include "PickerRobot.h"
#include "RobotController.h"
#include "Manager.h"

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
	Point unload_point(3, 8);

	Warehouse whouse("A", 10, 10);
	PickerRobot robot(3);
	robot.setSerialParameters(3, 9600);

	RobotController r_controller(robot, whouse);
	r_controller.setStartingPoint(start_point);
	r_controller.setUnloadingPoint(unload_point);
	r_controller.startRobot();

	Order order;
	order.amountOfItems = 1;
	order.compartmentPosition = 77;
	order.customerID = "1254";
	order.productID = "1111";
	order.priority = 1;
	order.truckNumber = 2;

	r_controller.processOrder(order);
	
#else

	Manager manager;
	manager.setup("wh_config.txt");
	manager.execute("Order_Picking_List.csv","Article_List.csv");

#endif


	write_footer();
	clog.rdbuf(oldbuf);

	return 0;

}
