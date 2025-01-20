#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <fstream>
#include <ctime>
#include <map>
using namespace std;

// ===== Global Variables =====
const float DEFAULT_VAT = 0.07;
const float DEFAULT_SERVICE_CHARGE = 0.1;
const float MEMBER_DISCOUNT = 0.12;
float vat = DEFAULT_VAT;
float serviceCharge = DEFAULT_SERVICE_CHARGE;
vector<string> menuList = {
    "Charcoal-boiled pork neck", "Fried Basil and Pork", "Papaya salad JoJo", 
    "Fried rice with Crab", "Spicy minced chicken salad", 
    "Spicy Shrimp Soup", "Stewed pork leg on rice", "Grilled Shrimp"
};
vector<float> menuPrices = {40.0, 40.0, 50.0, 60.0, 70.0, 70.0, 90.0, 300.0};
map<string, float> coupons = {{"DISCOUNT10", 0.10}, {"WELCOME20", 0.20}};

// ===== Function Prototypes =====
void displayMenu();
void configMode();
void calculateBill(vector<pair<int, int>> orders, bool isMember, string customerName, string couponCode, int splitBill);
void viewOrderHistory();
void saveOrderHistory(string customerName, vector<pair<int, int>> orders, float total, string couponCode);
void dynamicMenuUpdate();

int main() {
    bool loop = true;
    while (loop) {
        vector<pair<int, int>> orders; // {menuIndex, quantity}
        string customerName;
        bool isMember = false;
        char memberChoice;
        string input, couponCode;
        int splitBill = 1;

        // Display Menu
        displayMenu();

        // Get Orders
        cout << "Enter your order in the format (menu-quantity), e.g., 1-2 3-1:\n";
        getline(cin, input);

        // Check if Config Mode or View History
        if (input == "set") {
            configMode();
            continue;
        } else if (input == "history") {
            viewOrderHistory();
            continue;
        }

        // Parse Orders
        int menuIndex, quantity;
        size_t pos = 0;
        bool hasError = false;

        while ((pos = input.find(" ")) != string::npos || !input.empty()) {
            string order = input.substr(0, pos);
            size_t dash = order.find("-");
            if (dash == string::npos || order.empty()) {
                cout << "Invalid order format. Try again.\n";
                hasError = true;
                break;
            }
            menuIndex = stoi(order.substr(0, dash));
            quantity = stoi(order.substr(dash + 1));
            if (menuIndex < 1 || menuIndex > menuList.size() || quantity <= 0) {
                cout << "Invalid menu number or quantity. Try again.\n";
                hasError = true;
                break;
            }
            orders.push_back({menuIndex - 1, quantity});
            input.erase(0, (pos == string::npos ? input.length() : pos + 1));
        }

        if (hasError) continue;

        // Membership Check
        cout << "Do you have a membership card? [Y/N]: ";
        cin >> memberChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');

        if (memberChoice == 'Y' || memberChoice == 'y') {
            isMember = true;
            cout << "Enter member name: ";
            getline(cin, customerName);
        } else {
            cout << "Enter customer name (optional): ";
            getline(cin, customerName);
        }

        // Coupon Check
        cout << "Do you have a coupon code? [Enter or press ENTER to skip]: ";
        getline(cin, couponCode);
        if (!couponCode.empty() && coupons.find(couponCode) == coupons.end()) {
            cout << "Invalid coupon code. Skipping.\n";
            couponCode.clear();
        }

        // Split Bill Check
        cout << "How many people to split the bill? (Enter 1 for no split): ";
        cin >> splitBill;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        if (splitBill <= 0) splitBill = 1;

        // Calculate and Print Bill
        calculateBill(orders, isMember, customerName, couponCode, splitBill);

        // Ask to Continue
        char continueChoice;
        cout << "Do you want to use the program again? [Y/N]: ";
        cin >> continueChoice;
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        loop = (continueChoice == 'Y' || continueChoice == 'y');
    }

    cout << "Exiting the program. Thank you for using it!\n";
    return 0;
}

// ===== Function Definitions =====

void displayMenu() {
    cout << "\n========== Menu ==========\n";
    for (size_t i = 0; i < menuList.size(); ++i) {
        cout << "[" << i + 1 << "] " << setw(30) << left << menuList[i] 
             << "THB " << fixed << setprecision(2) << menuPrices[i] << "\n";
    }
    cout << "==========================\n";
    cout << "[set] Enter Config Mode\n[history] View Order History\n";
}

void configMode() {
    int choice;
    cout << "\nEntering Config Mode...\n";
    cout << "[1] Set VAT (%)\n";
    cout << "[2] Set Service Charge (%)\n";
    cout << "[3] Reset to Default Settings\n";
    cout << "[4] Update Menu\n";
    cout << "Select an option: ";
    cin >> choice;

    switch (choice) {
        case 1:
            cout << "Enter new VAT percentage: ";
            cin >> vat;
            vat /= 100;
            cout << "VAT updated to " << vat * 100 << "%\n";
            break;
        case 2:
            cout << "Enter new Service Charge percentage: ";
            cin >> serviceCharge;
            serviceCharge /= 100;
            cout << "Service Charge updated to " << serviceCharge * 100 << "%\n";
            break;
        case 3:
            vat = DEFAULT_VAT;
            serviceCharge = DEFAULT_SERVICE_CHARGE;
            cout << "Settings reset to default.\n";
            break;
        case 4:
            dynamicMenuUpdate();
            break;
        default:
            cout << "Invalid option. Returning to main menu.\n";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void dynamicMenuUpdate() {
    int choice;
    cout << "[1] Add Item\n[2] Remove Item\n[3] Update Item Price\n";
    cin >> choice;
    if (choice == 1) {
        string newItem;
        float newPrice;
        cout << "Enter new item name: ";
        cin.ignore();
        getline(cin, newItem);
        cout << "Enter price: ";
        cin >> newPrice;
        menuList.push_back(newItem);
        menuPrices.push_back(newPrice);
        cout << "Item added successfully.\n";
    } else if (choice == 2) {
        int index;
        displayMenu();
        cout << "Enter the item number to remove: ";
        cin >> index;
        if (index > 0 && index <= menuList.size()) {
            menuList.erase(menuList.begin() + index - 1);
            menuPrices.erase(menuPrices.begin() + index - 1);
            cout << "Item removed successfully.\n";
        } else {
            cout << "Invalid item number.\n";
        }
    } else if (choice == 3) {
        int index;
        float newPrice;
        displayMenu();
        cout << "Enter the item number to update price: ";
        cin >> index;
        if (index > 0 && index <= menuList.size()) {
            cout << "Enter new price: ";
            cin >> newPrice;
            menuPrices[index - 1] = newPrice;
            cout << "Price updated successfully.\n";
        } else {
            cout << "Invalid item number.\n";
        }
    } else {
        cout << "Invalid choice.\n";
    }
}

void calculateBill(vector<pair<int, int>> orders, bool isMember, string customerName, string couponCode, int splitBill) {
    float subtotal = 0;
    cout << "\n========== Bill ==========\n";
    if (!customerName.empty()) {
        cout << "Customer: " << customerName << "\n";
    }

    // Get current time
    time_t now = time(0);
    char* dt = ctime(&now);
    cout << "Date: " << dt;
    cout << "----------------------------------\n";
    for (const auto& order : orders) {
        int menuIndex = order.first;
        int quantity = order.second;
        float itemTotal = menuPrices[menuIndex] * quantity;
        subtotal += itemTotal;
        cout << menuList[menuIndex] << " x" << quantity 
             << " = " << fixed << setprecision(2) << itemTotal << " THB\n";
    }
    cout << "----------------------------------\n";

    float service = subtotal * serviceCharge;
    float tax = subtotal * vat;
    float discount = isMember ? (subtotal + service + tax) * MEMBER_DISCOUNT : 0;
    if (!couponCode.empty()) {
        discount += (subtotal + service + tax) * coupons[couponCode];
        cout << "Coupon Applied (" << couponCode << "): -" << (subtotal + service + tax) * coupons[couponCode] << " THB\n";
    }
    float grandTotal = subtotal + service + tax - discount;

    cout << "Subtotal        : " << fixed << setprecision(2) << subtotal << " THB\n";
    cout << "Service Charge  : " << service << " THB\n";
    cout << "VAT             : " << tax << " THB\n";
    if (isMember) {
        cout << "Member Discount : -" << discount << " THB\n";
    }
    cout << "Grand Total     : " << grandTotal << " THB\n";
    cout << "Per Person      : " << grandTotal / splitBill << " THB\n";
    cout << "==========================\n";

    saveOrderHistory(customerName, orders, grandTotal, couponCode);
}

void saveOrderHistory(string customerName, vector<pair<int, int>> orders, float total, string couponCode) {
    ofstream historyFile("order_history.txt", ios::app);
    time_t now = time(0);
    char* dt = ctime(&now);

    historyFile << "Date: " << dt;
    historyFile << "Customer: " << (customerName.empty() ? "Anonymous" : customerName) << "\n";
    for (const auto& order : orders) {
        historyFile << menuList[order.first] << " x" << order.second << "\n";
    }
    if (!couponCode.empty()) {
        historyFile << "Coupon Applied: " << couponCode << "\n";
    }
    historyFile << "Total: " << fixed << setprecision(2) << total << " THB\n";
    historyFile << "----------------------------------\n";
    historyFile.close();
}

void viewOrderHistory() {
    ifstream historyFile("order_history.txt");
    if (!historyFile) {
        cout << "No order history found.\n";
        return;
    }
    cout << "\n========== Order History ==========\n";
    string line;
    while (getline(historyFile, line)) {
        cout << line << "\n";
    }
    cout << "===================================\n";
    historyFile.close();
}
