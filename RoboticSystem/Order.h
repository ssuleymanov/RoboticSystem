#pragma once
#ifndef ORDER_H
#define ORDER_H

#include <string>

typedef struct {
	std::string	productID;
	std::string	warehouseID;
	int		compartment;
}Article;

struct Order {
	int		compartment;
	std::string	customerID;
	int		orderID;
	int		priority;
	std::string	productID;
	int		quantity;
	int		truckNr;
	std::string	warehouseID;
	bool operator<(const Order &rhs) const { return compartment < rhs.compartment; }
};

typedef struct Order Order;

#endif // !ORDER_H