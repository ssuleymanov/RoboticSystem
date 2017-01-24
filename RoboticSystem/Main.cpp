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
	
	system("del /Q Logs\\*.txt"); //Delete already existing log files
	filebuf os;
	os.open("Logs\\log.txt", fstream::app);
	streambuf* oldbuf = clog.rdbuf(&os);

	filebuf oss;
	oss.open("Logs\\err.txt", fstream::app);
	streambuf* oldbuff = cerr.rdbuf(&oss);

	write_header();

	Manager manager;
	string fileName, parameter;
	char param1;
	bool setupDone = false;
	

	cout << "(A)utomatic or (M)anual?\n";
	param1 = getchar();

	ifstream input_file("input.txt");
	string fileParams[3];

	for (int i = 0; i < 3; i++) {
		getline(input_file, parameter);
		fileParams[i] = parameter;
	}

	initscr();
	start_color();
	init_pair(3,COLOR_BLUE,COLOR_BLACK);
	init_pair(2, COLOR_WHITE, COLOR_BLACK);
	cbreak();
	curs_set(0);

	if (param1 == 'A' || param1 == 'a') {
		manager.setup(fileParams[0]); //"wh_config.txt"
		manager.readArticles(fileParams[1]);//"Article_List_XML.xml"
		manager.execute(fileParams[2]);//"Order_Picking_List.csv"
	}
	else if (param1 == 'M' || param1 == 'm') {
		manager.readArticles(fileParams[1]);
		char productID[20], quantity[5], validate[2];
		while (true) {
			printw("ProductID:  ");
			while (true) {
				getstr(productID);
				if (manager.productValid(productID)) { break; }
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
					printw("%s is not a number, please try again or press enter to stop\n", quantity);
					printw("Quantity:  ");
				}
				if (quantity[0] == '\0') { return (EXIT_FAILURE); }
			}
			noecho();
			if (!setupDone) { manager.setup(fileParams[0]); setupDone = true; }//		
			manager.manualControl(productID, q);//"manual_orders.txt"
			echo();
			clear();
			printw("Type Q to quit or press another key to select another product\n");
			getstr(validate);
			if (validate[0] == 'q' || validate[0] == 'Q') { break; }
			clear();
		}
	}
	else {
		cout << "Incorrect \n";
		getchar();
	}

	write_footer();
	clog.rdbuf(oldbuf);
	cerr.rdbuf(oldbuff);


	return 0;

}
