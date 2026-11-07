#include "ProductDatabase.h"
#include "Database/sqlite3.h"

#include <iostream>
#include <fstream>
#include <sstream>

// Global SQLite database connection.
sqlite3* db = nullptr;


// ------------------------------------------------------------
// Opens the SQLite database.
// ------------------------------------------------------------
bool openDatabase()
{
    int result = sqlite3_open("products.db", &db);

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to open database." << std::endl;
        return false;
    }

    std::cout << "Database connected successfully!" << std::endl;

    return true;
}


// ------------------------------------------------------------
// Creates the Products table if it does not already exist.
// ------------------------------------------------------------
bool createProductsTable()
{
    const char* sql = R"(
        CREATE TABLE IF NOT EXISTS Products (
            ProductID INTEGER PRIMARY KEY,
            Name TEXT NOT NULL,
            Category TEXT NOT NULL,
            Barcode TEXT,
            PLU INTEGER,
            Price REAL NOT NULL,
            IsWeighable INTEGER NOT NULL,
            Unit TEXT NOT NULL
        );
    )";

    char* errorMessage = nullptr;

    int result = sqlite3_exec(
        db,
        sql,
        nullptr,
        nullptr,
        &errorMessage
    );

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to create Products table: "
            << errorMessage << std::endl;

        sqlite3_free(errorMessage);

        return false;
    }

    std::cout << "Products table created successfully!"
        << std::endl;

    return true;
}


// ------------------------------------------------------------
// Inserts one Product into the SQLite database.
// ------------------------------------------------------------
bool ProductDatabase::insertProduct(const Product& product)
{
    const char* sql = R"(
        INSERT OR REPLACE INTO Products
        (ProductID, Name, Category, Barcode, PLU, Price, IsWeighable, Unit)
        VALUES (?, ?, ?, ?, ?, ?, ?, ?);
    )";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare product insert."
            << std::endl;

        return false;
    }

    // Product ID
    sqlite3_bind_int(
        statement,
        1,
        product.productID
    );

    // Product name
    sqlite3_bind_text(
        statement,
        2,
        product.name.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    // Product category
    sqlite3_bind_text(
        statement,
        3,
        product.category.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    // Barcode
    if (product.barcode.empty())
    {
        sqlite3_bind_null(statement, 4);
    }
    else
    {
        sqlite3_bind_text(
            statement,
            4,
            product.barcode.c_str(),
            -1,
            SQLITE_TRANSIENT
        );
    }

    // PLU
    if (product.PLU.empty())
    {
        sqlite3_bind_null(statement, 5);
    }
    else
    {
        sqlite3_bind_text(
            statement,
            5,
            product.PLU.c_str(),
            -1,
            SQLITE_TRANSIENT
        );
    }

    // Price
    sqlite3_bind_double(
        statement,
        6,
        product.price
    );

    // SQLite stores bool values as 0 or 1.
    sqlite3_bind_int(
        statement,
        7,
        product.isWeighable ? 1 : 0
    );

    // Unit
    sqlite3_bind_text(
        statement,
        8,
        product.unit.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    // Execute INSERT.
    result = sqlite3_step(statement);

    sqlite3_finalize(statement);

    if (result != SQLITE_DONE)
    {
        std::cout << "Failed to insert product."
            << std::endl;

        return false;
    }

    return true;
}


// ------------------------------------------------------------
// Loads products from a CSV file and stores them in SQLite.
// ------------------------------------------------------------
bool ProductDatabase::loadProductsFromCSV(
    const std::string& filename)
{
    std::ifstream file(filename);

    if (!file.is_open())
    {
        std::cout << "Failed to open CSV file."
            << std::endl;

        return false;
    }

    std::string line;

    // Skip the CSV header.
    std::getline(file, line);

    while (std::getline(file, line))
    {
        // Ignore empty lines in the CSV file.
        if (line.empty())
        {
            continue;
        }

        Product product;

        // Temporary strings used while reading CSV values.
        std::string productID;
        std::string price;
        std::string isWeighable;

        std::stringstream ss(line);

        // Read each CSV column.
        std::getline(ss, productID, ',');
        std::getline(ss, product.name, ',');
        std::getline(ss, product.category, ',');
        std::getline(ss, product.barcode, ',');
        std::getline(ss, product.PLU, ',');
        std::getline(ss, price, ',');
        std::getline(ss, isWeighable, ',');
        std::getline(ss, product.unit, ',');

        // Convert text values into the correct data types.
        product.productID = std::stoi(productID);
        product.price = std::stod(price);
        product.isWeighable = (isWeighable == "true");

        // Insert the product into SQLite.
        if (!insertProduct(product))
        {
            std::cout << "Failed to insert: "
                << product.name << std::endl;
        }
    }

    file.close();

    std::cout << "Products loaded from CSV successfully!"
        << std::endl;

    return true;
}

// ------------------------------------------------------------
// Displays all products stored in the SQLite database.
// ------------------------------------------------------------
void ProductDatabase::displayProducts() const
{
    const char* sql = R"(
        SELECT ProductID, Name, Category, Price, IsWeighable, Unit
        FROM Products;
    )";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare product display."
            << std::endl;

        return;
    }

    std::cout << "\n==============================" << std::endl;
    std::cout << "        PRODUCT LIST" << std::endl;
    std::cout << "==============================" << std::endl;

    while (sqlite3_step(statement) == SQLITE_ROW)
    {
        int productID =
            sqlite3_column_int(statement, 0);

        const char* name =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 1)
                );

        const char* category =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 2)
                );

        double price =
            sqlite3_column_double(statement, 3);

        bool isWeighable =
            sqlite3_column_int(statement, 4) != 0;

        const char* unit =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 5)
                );

        std::cout << "\nID: " << productID
            << "\nName: " << name
            << "\nCategory: " << category
            << "\nPrice: $" << price
            << "\nSold by: "
            << (isWeighable ? "Weight" : "Unit")
            << " (" << unit << ")"
            << "\n------------------------------";
    }

    sqlite3_finalize(statement);

    std::cout << "\n" << std::endl;
}

// ------------------------------------------------------------
// Searches SQLite for a product using its barcode.
// Returns true if the product is found.
// ------------------------------------------------------------
bool ProductDatabase::findByBarcode(
    const std::string& barcode,
    Product& product) const
{
    const char* sql = R"(
        SELECT ProductID, Name, Category, Barcode, PLU,
               Price, IsWeighable, Unit
        FROM Products
        WHERE Barcode = ?;
    )";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare barcode search."
            << std::endl;

        return false;
    }

    // Insert the barcode into the SQL query.
    sqlite3_bind_text(
        statement,
        1,
        barcode.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        // Copy database values into the Product object.
        product.productID =
            sqlite3_column_int(statement, 0);

        product.name =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 1)
                );

        product.category =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 2)
                );

        const unsigned char* barcodeValue =
            sqlite3_column_text(statement, 3);

        if (barcodeValue)
        {
            product.barcode =
                reinterpret_cast<const char*>(
                    barcodeValue
                    );
        }
        else
        {
            product.barcode = "";
        }

        const unsigned char* pluValue =
            sqlite3_column_text(statement, 4);

        if (pluValue)
        {
            product.PLU =
                reinterpret_cast<const char*>(
                    pluValue
                    );
        }
        else
        {
            product.PLU = "";
        }

        product.price =
            sqlite3_column_double(statement, 5);

        product.isWeighable =
            sqlite3_column_int(statement, 6) != 0;

        product.unit =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 7)
                );

        sqlite3_finalize(statement);

        return true;
    }

    sqlite3_finalize(statement);

    return false;
}


// ------------------------------------------------------------
// Searches SQLite for a product using its PLU.
// Returns true if the product is found.
// ------------------------------------------------------------
bool ProductDatabase::findByPLU(
    const std::string& plu,
    Product& product) const
{
    const char* sql = R"(
        SELECT ProductID, Name, Category, Barcode, PLU,
               Price, IsWeighable, Unit
        FROM Products
        WHERE PLU = ?;
    )";

    sqlite3_stmt* statement = nullptr;

    int result = sqlite3_prepare_v2(
        db,
        sql,
        -1,
        &statement,
        nullptr
    );

    if (result != SQLITE_OK)
    {
        std::cout << "Failed to prepare PLU search."
            << std::endl;

        return false;
    }

    // Insert the PLU into the SQL query.
    sqlite3_bind_text(
        statement,
        1,
        plu.c_str(),
        -1,
        SQLITE_TRANSIENT
    );

    result = sqlite3_step(statement);

    if (result == SQLITE_ROW)
    {
        // Copy database values into the Product object.
        product.productID =
            sqlite3_column_int(statement, 0);

        product.name =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 1)
                );

        product.category =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 2)
                );

        const unsigned char* barcodeValue =
            sqlite3_column_text(statement, 3);

        if (barcodeValue)
        {
            product.barcode =
                reinterpret_cast<const char*>(
                    barcodeValue
                    );
        }
        else
        {
            product.barcode = "";
        }

        const unsigned char* pluValue =
            sqlite3_column_text(statement, 4);

        if (pluValue)
        {
            product.PLU =
                reinterpret_cast<const char*>(
                    pluValue
                    );
        }
        else
        {
            product.PLU = "";
        }

        product.price =
            sqlite3_column_double(statement, 5);

        product.isWeighable =
            sqlite3_column_int(statement, 6) != 0;

        product.unit =
            reinterpret_cast<const char*>(
                sqlite3_column_text(statement, 7)
                );

        sqlite3_finalize(statement);

        return true;
    }

    sqlite3_finalize(statement);

    return false;
}