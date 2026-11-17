#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <cctype>

using namespace std;

struct User {
    string username;
    string passwordHash;
    string email;
};

class AuthSystem {
private:
    const string filename = "users_v2.txt";

    string hashPassword(const string& password) {
        hash<string> hasher;
        return to_string(hasher(password + "SALT_KEY_2026"));
    }

    bool isStrongPassword(const string& password) {
        if (password.length() < 6) return false;
        bool hasDigit = false, hasAlpha = false;
        for (char c : password) {
            if (isdigit(c)) hasDigit = true;
            if (isalpha(c)) hasAlpha = true;
        }
        return hasDigit && hasAlpha;
    }

    vector<User> loadUsers() {
        vector<User> users;
        ifstream file(filename);
        User u;
        while (file >> u.username >> u.passwordHash >> u.email) {
            users.push_back(u);
        }
        file.close();
        return users;
    }

    void saveAllUsers(const vector<User>& users) {
        ofstream file(filename, ios::trunc);
        for (const auto& u : users) {
            file << u.username << " " << u.passwordHash << " " << u.email << "\n";
        }
        file.close();
    }

    bool userExists(const string& username) {
        vector<User> users = loadUsers();
        for (const auto& u : users) {
            if (u.username == username) return true;
        }
        return false;
    }

public:
    void registerUser() {
        string username, password, email;
        cout << "\n--- User Registration (v2.0) ---\n";
        cout << "Enter Username (no spaces): ";
        cin >> username;

        if (userExists(username)) {
            cout << "Error: Username already taken.\n";
            return;
        }

        cout << "Enter Email: ";
        cin >> email;

        while (true) {
            cout << "Enter Password (min 6 chars, at least 1 letter & 1 number): ";
            cin >> password;
            if (isStrongPassword(password)) {
                break;
            }
            cout << "Weak password. Please meet the requirements.\n";
        }

        ofstream file(filename, ios::app);
        if (file.is_open()) {
            file << username << " " << hashPassword(password) << " " << email << "\n";
            file.close();
            cout << "Registration successful! Account created securely.\n";
        } else {
            cout << "Error: Could not write to database.\n";
        }
    }

    void loginUser() {
        string username, password;
        int attempts = 0;
        const int maxAttempts = 3;

        cout << "\n--- User Login ---\n";
        cout << "Enter Username: ";
        cin >> username;

        vector<User> users = loadUsers();
        int userIndex = -1;
        for (size_t i = 0; i < users.size(); ++i) {
            if (users[i].username == username) {
                userIndex = i;
                break;
            }
        }

        if (userIndex == -1) {
            cout << "Error: User not found.\n";
            return;
        }

        while (attempts < maxAttempts) {
            cout << "Enter Password: ";
            cin >> password;

            if (users[userIndex].passwordHash == hashPassword(password)) {
                cout << "Login successful! Welcome back, " << username << ".\n";
                userDashboard(username);
                return;
            } else {
                attempts++;
                cout << "Incorrect password. Attempts remaining: " << (maxAttempts - attempts) << "\n";
            }
        }

        cout << "Account temporarily locked due to too many failed attempts.\n";
    }

    void changePassword(const string& username) {
        string oldPass, newPass;
        cout << "\n--- Change Password ---\n";
        cout << "Enter Current Password: ";
        cin >> oldPass;

        vector<User> users = loadUsers();
        for (auto& u : users) {
            if (u.username == username) {
                if (u.passwordHash != hashPassword(oldPass)) {
                    cout << "Error: Current password is incorrect.\n";
                    return;
                }

                while (true) {
                    cout << "Enter New Password (min 6 chars, 1 letter & 1 number): ";
                    cin >> newPass;
                    if (isStrongPassword(newPass)) break;
                    cout << "Weak password. Try again.\n";
                }

                u.passwordHash = hashPassword(newPass);
                saveAllUsers(users);
                cout << "Password updated successfully!\n";
                return;
            }
        }
    }

    void deleteAccount(const string& username) {
        char confirm;
        cout << "Are you sure you want to delete your account? (y/n): ";
        cin >> confirm;
        if (confirm != 'y' && confirm != 'Y') {
            cout << "Account deletion canceled.\n";
            return;
        }

        vector<User> users = loadUsers();
        vector<User> updatedUsers;
        for (const auto& u : users) {
            if (u.username != username) {
                updatedUsers.push_back(u);
            }
        }
        saveAllUsers(updatedUsers);
        cout << "Account deleted permanently.\n";
    }

    void userDashboard(const string& username) {
        int choice;
        while (true) {
            vector<User> users = loadUsers();
            string currentEmail = "";
            for (const auto& u : users) {
                if (u.username == username) currentEmail = u.email;
            }

            cout << "\n=== Dashboard (" << username << ") ===\n";
            cout << "1. View Profile Details\n";
            cout << "2. Change Password\n";
            cout << "3. Delete Account\n";
            cout << "4. Logout\n";
            cout << "Choose an option: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input.\n";
                continue;
            }

            if (choice == 1) {
                cout << "\n--- Profile Info ---\n";
                cout << "Username: " << username << "\n";
                cout << "Email:    " << currentEmail << "\n";
            } else if (choice == 2) {
                changePassword(username);
            } else if (choice == 3) {
                deleteAccount(username);
                break;
            } else if (choice == 4) {
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
        cout << "\n====================================\n";
        cout << " Login & Registration System (v2.0)\n";
        cout << "====================================\n";
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
