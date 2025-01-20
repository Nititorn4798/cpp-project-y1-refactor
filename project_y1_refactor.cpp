#include <iostream>
#include <string>
#include <vector>
#include <iomanip>
#include <limits>
#include <ctime>
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

// ===== Function Prototypes =====
void displayMenu();
void configMode();
void calculateBill(vector<pair<int, int>> orders, bool isMember, string customerName);
void validateInput();

int main() {
    bool loop = true;
    while (loop) {
        vector<pair<int, int>> orders; // {menuIndex, quantity}
        string customerName;
        bool isMember = false;
        char memberChoice;
        string input;

        // Display Menu
        displayMenu();

        // Get Orders
        cout << "Enter your order in the format (menu-quantity), e.g., 1-2 3-1:\n";
        getline(cin, input);

        // Check if Config Mode
        if (input == "set") {
            configMode();
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

        // Calculate and Print Bill
        calculateBill(orders, isMember, customerName);

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
}

void configMode() {
    int choice;
    cout << "\nEntering Config Mode...\n";
    cout << "[1] Set VAT (%)\n";
    cout << "[2] Set Service Charge (%)\n";
    cout << "[3] Reset to Default Settings\n";
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
        default:
            cout << "Invalid option. Returning to main menu.\n";
    }
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
}

void calculateBill(vector<pair<int, int>> orders, bool isMember, string customerName) {
    float subtotal = 0;
    cout << "\n========== Bill ==========\n";
    if (!customerName.empty()) {
        cout << "Customer: " << customerName << "\n";
    }
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
    float grandTotal = subtotal + service + tax - discount;

    cout << "Subtotal        : " << fixed << setprecision(2) << subtotal << " THB\n";
    cout << "Service Charge  : " << service << " THB\n";
    cout << "VAT             : " << tax << " THB\n";
    if (isMember) {
        cout << "Member Discount : -" << discount << " THB\n";
    }
    cout << "Grand Total     : " << grandTotal << " THB\n";
    cout << "==========================\n";
}
