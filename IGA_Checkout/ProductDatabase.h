#pragma once

#include <string>
#include "Product.h"

// Handles all operations related to the product database.
class ProductDatabase
{
public:

    // Searches the SQLite database for a product using its barcode.
    // Returns true if a product is found.
    bool findByBarcode(
        const std::string& barcode,
        Product& product
    ) const;

    // Searches the SQLite database for a product using its PLU.
    // Returns true if a product is found.
    bool findByPLU(
        const std::string& plu,
        Product& product
    ) const;

    // Displays products stored in the database.
    void displayProducts() const;

    // Inserts a product into the SQLite database.
    bool insertProduct(const Product& product);

    // Loads products from a CSV file and inserts them into SQLite.
    bool loadProductsFromCSV(const std::string& filename);
};