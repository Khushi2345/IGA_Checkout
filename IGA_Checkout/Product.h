#pragma once
#include <string>

class Product 
{
public:
	int productID;
	std::string name;
	std::string category;
	std::string barcode;
	std::string PLU;
	double price;
	bool isWeighable;
	std::string unit;
};