#include <iostream>   
#include <vector>     
#include <string>     
#include <algorithm> 
#include <map>        
#include <sstream>   
#include <iomanip>   
#include <cmath>     

using namespace std;
// STRUCT: Item
// Represents a single product
struct Item {
    string code;        // Unique identifier (e.g., A1, B2)
    string name;        // Product name
    string category;    // Product category (e.g., Snacks, Hot Drinks)
    int priceCents;    // Price stored in cents to avoid floating-point errors
    int stock;          // Number of items available
};

// CLASS: VendingMachine
// Controls vending machine logic
class VendingMachine {
private:
    vector<Item> inventory;  // Stores all vending machine items

        // Converts cents into a formatted £X.XX string
       static string centsToStr(int cents) {
        ostringstream oss;
        oss << fixed << setprecision(2) << (cents / 100.0);
        return "£" + oss.str();
    }

       // Converts user-entered money into cents
    // Accepts inputs such as "1.20", "£2", " 0.50 "
    // Returns -1 if the input is invalid
     static int parseMoneyToCents(const string &s) {
        try {
            string t = s;

            // Remove spaces from input
            t.erase(remove(t.begin(), t.end(), ' '), t.end());

            // Remove pound (£) symbols
            size_t pos = 0;
            while ((pos = t.find("£", pos)) != string::npos) {
                t.erase(pos, 1);
            }

            // Reject empty input
            if (t.empty()) return -1;

            // Convert string to numeric value
            double val = stod(t);

            // Reject negative values
            if (val < 0) return -1;

            // Convert pounds to cents
            int cents = static_cast<int>(round(val * 100));
            return cents;
        }
        catch (...) {
            // Any conversion error is treated as invalid input
            return -1;
        }
    }

     // Converts a string to uppercase
    // Used for case-insensitive input handling
        static string toUpper(string s) {
        transform(s.begin(), s.end(), s.begin(),
                  [](unsigned char c) {
                      return static_cast<char>(toupper(c));
                  });
        return s;
    }

    // Searches inventory for an item code
    // Returns pointer to item or nullptr if not found
    Item* findItemByCode(const string &code) {
        string upperCode = toUpper(code);
        for (auto &it : inventory) {
            if (toUpper(it.code) == upperCode)
                return &it;
        }
        return nullptr;
    }

        // Displays menu grouped by category
      void displayMenu() {
        cout << "\n=== VENDING MACHINE MENU ===\n";

        // Group items by category
        map<string, vector<Item>> groups;
        for (auto &it : inventory) {
            groups[it.category].push_back(it);
        }

        // Display grouped items
        for (auto &g : groups) {
            cout << "\n-- " << g.first << " --\n";
            for (auto &item : g.second) {
                cout << item.code << ": " << item.name
                     << " (" << centsToStr(item.priceCents) << ")"
                     << " [stock: " << item.stock << "]\n";
            }
        }

        cout << "\nEnter item code (e.g. A1) or type EXIT to quit.\n";
    }

       // Returns change to the user
        void dispenseChange(int changeCents) {
        if (changeCents <= 0) {
            cout << "No change to return.\n";
            return;
        }
        cout << "Change returned: " << centsToStr(changeCents) << "\n";
    }

        // Suggests a cheaper item from the same category
       Item* suggestItem(const Item &bought) {
        Item* bestSuggestion = nullptr;

        for (auto &it : inventory) {
            if (it.category == bought.category &&
                it.code != bought.code &&
                it.stock > 0) {

                if (!bestSuggestion ||
                    it.priceCents < bestSuggestion->priceCents) {
                    bestSuggestion = &it;
                }
            }
        }
        return bestSuggestion;
    }

public:
        // Constructor: Initializes inventory
     VendingMachine() {
        inventory = {
            {"A1","Tea","Hot Drinks",        90,10},
            {"A2","Coffee","Hot Drinks",    120, 8},
            {"A3","Hot Chocolate","Hot Drinks",110,5},
            {"B1","Crisps","Snacks",         85, 6},
            {"B2","Savoury Biscuits","Snacks",95,4},
            {"B3","Chocolate Bar","Snacks", 120,2}
        };
    }

      // Main program loop
        void run() {
        cout << "Welcome to the Utility Vending Machine!\n";

        while (true) {
            displayMenu();

            string choice;
            cout << "Choice: ";
            if (!getline(cin, choice)) break;
            if (choice.empty()) continue;

            // Exit condition
            if (toUpper(choice) == "EXIT") break;

            // Validate item code
            Item* item = findItemByCode(choice);
            if (!item) {
                cout << "Invalid code. Please try again.\n";
                continue;
            }

            // Check stock availability
            if (item->stock <= 0) {
                cout << "Sorry, " << item->name << " is out of stock.\n";
                continue;
            }

            // Display selected item
            cout << "Selected: " << item->name
                 << " — Price: " << centsToStr(item->priceCents) << "\n";

            int paid = 0;

            // Payment loop
            while (paid < item->priceCents) {
                cout << "Insert money (e.g. 1.20) or type CANCEL. "
                     << "Inserted so far: " << centsToStr(paid) << "\n";

                string moneyStr;
                if (!getline(cin, moneyStr)) {
                    cout << "Input ended unexpectedly.\n";
                    return;
                }
                if (moneyStr.empty()) continue;

                // Cancel transaction
                if (toUpper(moneyStr) == "CANCEL") {
                    cout << "Transaction cancelled. Returning: "
                         << centsToStr(paid) << "\n";
                    paid = 0;
                    break;
                }

                // Parse inserted money
                int c = parseMoneyToCents(moneyStr);
                if (c < 0) {
                    cout << "Invalid money format. "
                         << "Please enter values like 1.20 or 0.50\n";
                    continue;
                }

                paid += c;

                if (paid < item->priceCents) {
                    cout << "Amount remaining: "
                         << centsToStr(item->priceCents - paid) << "\n";
                }
            }

            if (paid == 0) continue;

            // Complete purchase
            int change = paid - item->priceCents;
            cout << "Dispensing: " << item->name
                 << " (" << item->code << ")\n";
            item->stock--;

            dispenseChange(change);

            // Suggest another item
            Item* suggestion = suggestItem(*item);
            if (suggestion) {
                cout << "Suggestion: You might also like "
                     << suggestion->name << " (" << suggestion->code
                     << ") for " << centsToStr(suggestion->priceCents)
                     << ".\n";
            }

            // Ask to continue
            cout << "Would you like to buy another item? (Y/N): ";
            string yn;
            if (!getline(cin, yn)) break;
            yn = toUpper(yn);
            if (yn.empty() || yn[0] != 'Y') break;
        }

        cout << "Thank you for using the Utility Vending Machine. Goodbye!\n";
    }
};

// MAIN FUNCTION
// Program entry point
int main() {
    VendingMachine vm;  // Create vending machine instance
    vm.run();           // Start program
    return 0;           // Exit successfully
}
