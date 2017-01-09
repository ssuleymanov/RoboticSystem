#pragma once
#ifndef ORDER_H
#define ORDER_H

#include <string>

struct Order {
	std::string productID;
	std::string customerID;
	int priority;
	int compartmentPosition;
	int truckNumber;
	int amountOfItems;
};

#endif // !ORDER_H