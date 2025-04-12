#include <iostream>
#include <vector>
#include <string>
#include <iomanip>

using namespace std;

struct Item {
    int itemid;
    string name;
    string des;
    string loc;
    int day, month, year;
};

class LostFound {
protected:
    vector<Item> lostItems;
    vector<Item> foundItems;

public:
    void addLostItem(Item item) {
        lostItems.push_back(item);
        cout << "Lost item added successfully.\n";
    }

    void addFoundItem(Item item) {
        foundItems.push_back(item);
        cout << "Found item added successfully.\n";
    }

    void viewLostItems() {
        cout << "\n--- Lost Items ---\n";
        for (const auto& item : lostItems) {
            displayItem(item);
        }
    }

    void viewFoundItems() {
        cout << "\n--- Found Items ---\n";
        for (const auto& item : foundItems) {
            displayItem(item);
        }
    }

    void displayItem(const Item& item) {
        cout << "ID: " << item.itemid << "\n"
             << "Name: " << item.name << "\n"
             << "Description: " << item.des << "\n"
             << "Location: " << item.loc << "\n"
             << "Date: " << item.day << "/" << item.month << "/" << item.year << "\n\n";
    }
};

class User : public LostFound {
public:
    void reportLostItem() {
        Item item = inputItemDetails();
        addLostItem(item);
    }

    void reportFoundItem() {
        Item item = inputItemDetails();
        addFoundItem(item);
    }

    Item inputItemDetails() {
        Item item;
        cout << "Enter item ID: "; cin >> item.itemid;
        cin.ignore();
        cout << "Enter item name: "; getline(cin, item.name);
        cout << "Enter description: "; getline(cin, item.des);
        cout << "Enter location: "; getline(cin, item.loc);
        cout << "Enter date.month.year: "; cin >> item.day >> item.month >> item.year;
        return item;
    }
};
class Admin : protected LostFound {
public:
    void adminViewLost() {
        viewLostItems();
    }

    void adminViewFound() {
        viewFoundItems();
    }
};
int main() {
    User user;
    Admin admin;

    int choice;
    while (true) {
        cout << "\n--- Lost and Found Hub ---\n";
        cout << "1. Report Lost Item\n";
        cout << "2. Report Found Item\n";
        cout << "3. View Lost Items (Admin)\n";
        cout << "4. View Found Items (Admin)\n";
        cout << "5. Exit\n";
        cout << "Enter your choice: ";
        cin >> choice;
        switch (choice) {
        case 1:
            user.reportLostItem();
            break;
        case 2:
            user.reportFoundItem();
            break;
        case 3:
            admin.adminViewLost();
            break;
        case 4:
            admin.adminViewFound();
            break;
        case 5:
            cout << "Exit op\n";
            return 0;
        default:
            cout << "Invalid choice.\n";
        }
    }

    return 0;
}
