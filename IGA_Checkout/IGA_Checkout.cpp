#include <iostream>
#include <string>
#include "ProductDatabase.h"
#include "Cart.h"

// Functions for connecting to and setting up the SQLite database.
bool openDatabase();
bool createProductsTable();

int main()
{
    // --------------------------------------------------
    // 1. Open the SQLite database
    // --------------------------------------------------

    if (!openDatabase())
    {
        return 1;
    }

    // --------------------------------------------------
    // 2. Create the Products table if it doesn't exist
    // --------------------------------------------------

    if (!createProductsTable())
    {
        return 1;
    }

    // Create our ProductDatabase object.
    ProductDatabase database;
    Cart cart;
    // --------------------------------------------------
    // 3. Load product information from the CSV file
    //    into the SQLite database.
    // --------------------------------------------------

    if (!database.loadProductsFromCSV("Data/products.csv"))
    {
        return 1;
    }

    // --------------------------------------------------
    // 4. Display the database test menu
    // --------------------------------------------------

    int choice = 0;

    while (choice != 5)
    {
        std::cout << "\n==============================" << std::endl;
        std::cout << "         IGA CHECKOUT" << std::endl;
        std::cout << "==============================" << std::endl;

        std::cout << "1. Scan Barcode" << std::endl;
        std::cout << "2. Enter PLU" << std::endl;
        std::cout << "3. View Cart" << std::endl;
        std::cout << "4. Finish Shopping" << std::endl;
        std::cout << "5. Exit" << std::endl;

        std::cout << "Choose option: ";
        std::cin >> choice;

        // --------------------------------------------------
        // Scan product using barcode
        // --------------------------------------------------

        if (choice == 1)
        {
            std::string barcode;

            std::cout << "\nScan barcode: ";
            std::cin >> barcode;

            Product result;

            if (database.findByBarcode(barcode, result))
            {
                cart.addItem(result);

                std::cout << "\nProduct found: "
                    << result.name << std::endl;

                std::cout << "Added to cart." << std::endl;

                std::cout << "Current total: $"
                    << cart.calculateTotal()
                    << std::endl;
            }
            else
            {
                std::cout << "\nProduct not found." << std::endl;
            }
        }

        // --------------------------------------------------
        // Find product using PLU
        // --------------------------------------------------

        else if (choice == 2)
        {
            std::string PLU;

            std::cout << "\nEnter PLU: ";
            std::cin >> PLU;

            Product result;

            if (database.findByPLU(PLU, result))
            {
                cart.addItem(result);

                std::cout << "\nProduct found: "
                    << result.name << std::endl;

                std::cout << "Added to cart." << std::endl;

                std::cout << "Current total: $"
                    << cart.calculateTotal()
                    << std::endl;
            }
            else
            {
                std::cout << "\nProduct not found." << std::endl;
            }
        }

        // --------------------------------------------------
        // Display current cart
        // --------------------------------------------------

        else if (choice == 3)
        {
            cart.displayCart();

            std::cout << "\nTotal: $"
                << cart.calculateTotal()
                << std::endl;
        }

        // --------------------------------------------------
        // Finish shopping
        // --------------------------------------------------

        else if (choice == 4)
        {
            std::cout << "\n==============================" << std::endl;
            std::cout << "       SHOPPING FINISHED" << std::endl;
            std::cout << "==============================" << std::endl;

            std::cout << "Items: "
                << cart.getItemCount()
                << std::endl;

            std::cout << "Total: $"
                << cart.calculateTotal()
                << std::endl;

            std::cout << "\nPayment system will be added next."
                << std::endl;
        }

        // --------------------------------------------------
        // Exit
        // --------------------------------------------------

        else if (choice == 5)
        {
            std::cout << "\nExiting IGA Checkout..." << std::endl;
        }

        else
        {
            std::cout << "\nInvalid choice. Please try again."
                << std::endl;
        }
    }
}