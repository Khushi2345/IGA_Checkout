#pragma once
#include <vector>
#include "Product.h"

class Cart
{
private:
    // Stores the products currently being purchased.
    std::vector<Product> items;

public:
    // Adds a product to the cart.
    void addItem(const Product& product);

    // Displays all products currently in the cart.
    void displayCart() const;

    // Calculates the current total price of the cart.
    double calculateTotal() const;

    // Returns the number of products currently in the cart.
    int getItemCount() const;
};
