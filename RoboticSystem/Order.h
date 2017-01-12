#pragma once
#ifndef ORDER_H
#define ORDER_H

#include <string>

typedef struct {
	std::string	productID;
	std::string	warehouseID;
	int		compartment;
}Article;

typedef struct {
	int		compartment;
	std::string	customerID;
	int		orderID;
	int		priority;
	std::string	productID;
	int		quantity;
	int		truckNr;
	std::string	warehouseID;
}Order;


//struct Order {
//	std::string productID;
//	std::string customerID;
//	int priority;
//	int compartmentPosition;
//	int truckNumber;
//	int amountOfItems;
//};


#endif // !ORDER_H