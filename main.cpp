#include <iostream>
#include <fstream>
#include <string>

using namespace std;

class AuthSystem {
private:
    const string filename = "users.txt";

    bool userExists(const string& username) {
        ifstream file(filename);
        string storedUser, storedPass;
        while (file >> storedUser >> storedPass) {
            if (storedUser == username) {
                return true;
            }
        }
        return false;
    }

public:
    void registerUser() {
        string username, password;
        cout << "\n--- User Registration ---\n";
        cout << "Enter Username (no spaces): ";
        cin >> username;

        if (userExists(username)) {
            cout << "Error: Username already exists.\n";
            return;
        }

        cout << "Enter Password (no spaces): ";
        cin >> password;

        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << username << " " << password << "\n";
            file.close();
            cout << "Registration successful!\n";
        } else {
            cout << "Error: Unable to open database file.\n";
        }
    }

    void loginUser() {
        string username, password;
        cout << "\n--- User Login ---\n";
        cout << "Enter Username: ";
        cin >> username;
        cout << "Enter Password: ";
        cin >> password;

        ifstream file(filename);
        string storedUser, storedPass;
        bool authenticated = false;

        while (file >> storedUser >> storedPass) {
            if (storedUser == username && storedPass == password) {
                authenticated = true;
                break;
            }
        }
        file.close();

        if (authenticated) {
            cout << "Login successful! Welcome, " << username << ".\n";
            userDashboard(username);
        } else {
            cout << "Invalid username or password.\n";
        }
    }

    void userDashboard(const string& username) {
        int choice;
        while (true) {
            cout << "\n=== Dashboard (" << username << ") ===\n";
            cout << "1. View Profile\n";
            cout << "2. Logout\n";
            cout << "Choose an option: ";
            cin >> choice;

            if (choice == 1) {
                cout << "Logged in user: " << username << "\n";
            } else if (choice == 2) {
                cout << "Logged out successfully.\n";
                break;
            } else {
                cout << "Invalid choice. Try again.\n";
            }
        }
    }
};

int main() {
    AuthSystem auth;
    int choice;

    while (true) {
        cout << "\n==============================\n";
        cout << " Login & Registration System\n";
        cout << "==============================\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Exit\n";
        cout << "Enter your choice: ";
        
        if (!(cin >> choice)) {
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Invalid input. Please enter a number.\n";
            continue;
        }

        switch (choice) {
            case 1:
                auth.registerUser();
                break;
            case 2:
                auth.loginUser();
                break;
            case 3:
                cout << "Exiting program.\n";
                return 0;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    }
} 
 






