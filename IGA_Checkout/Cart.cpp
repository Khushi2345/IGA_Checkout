#include "Cart.h"
#include <iostream>

// Adds a product to the cart.
void Cart::addItem(const Product& product)
{
    items.push_back(product);
}

// Displays all products currently in the cart.
void Cart::displayCart() const
{
    std::cout << "\n==============================" << std::endl;
    std::cout << "          YOUR CART" << std::endl;
    std::cout << "==============================" << std::endl;

    for (const Product& product : items)
    {
        std::cout << product.name
            << " - $" << product.price
            << std::endl;
    }
}

// Calculates the total price of all products in the cart.
double Cart::calculateTotal() const
{
    double total = 0.0;

    for (const Product& product : items)
    {
        total += product.price;
    }

    return total;
}

// Returns the number of products in the cart.
int Cart::getItemCount() const
{
    return static_cast<int>(items.size());
}