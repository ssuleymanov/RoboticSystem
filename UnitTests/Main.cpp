#include <gtest\gtest.h>
#include <gmock\gmock.h>

#include "..\RoboticSystem\Warehouse.h"
#include "..\RoboticSystem\Order.h"
#include "..\RoboticSystem\Point.h"
#include "..\RoboticSystem\Printer.h"


TEST(warehouseTest, compartmentTest) {
	Warehouse wh("A",20,4);
	Order orders[3];
	orders[0] = { 1, "1234", 1, 1, "5678", 2, 1, "A" };
	orders[1] = { 30, "22", 1, 2, "6", 2, 1, "A" };
	orders[2] = { 80, "545", 2, 1, "23", 2, 1, "A" };
	int x[3] = { 1, 2, 4 };
	int y[3] = { 1, 10, 20 };

	for (int i = 0; i < 3; i++) {
		Point p = wh.getCompartmentPosition(orders[i]);
		EXPECT_EQ(x[i], p.getX());
		EXPECT_EQ(y[i], p.getY());
	}
}

TEST(printerTest, addWindowTest) {
	Warehouse wh1("A", 20, 4);
	Warehouse wh2("A", 15, 5);
	Warehouse wh3("B", 6, 9);
	Warehouse wh4("C", 10, 6);
	Printer* printer = Printer::getInstance();
	EXPECT_EQ(20, printer->addWindow(wh1, 0));
	EXPECT_EQ(25, printer->addWindow(wh2, 0));
	EXPECT_EQ(60, printer->addWindow(wh3, 15));
	EXPECT_EQ(63, printer->addWindow(wh4, 33));
}