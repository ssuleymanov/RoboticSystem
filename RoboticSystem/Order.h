#pragma once
#ifndef ORDER_H
#define ORDER_H

#include <string>

typedef struct {
	string	productID;
	string	warehouseID;
	int		compartment;
}Article;

typedef struct {
	int		compartment;
	string	customerID;
	int		orderID;
	int		priority;
	string	productID;
	int		quantity;
	int		truckNr;
	string	warehouseID;
}Order;

#endif // !ORDER_H

