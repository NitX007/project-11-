#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <functional>
#include <cctype>
#include <ctime>

using namespace std;

struct User {
    string username;
    string passwordHash;
    string email;
    string role;
    string securityAnswerHash;
};

class AuthSystem {
private:
    const string userFile = "users_v3.txt";
    const string logFile = "activity.log";

    string getCurrentTime() {
        time_t now = time(0);
        string dt = ctime(&now);
        if (!dt.empty() && dt.back() == '\n') {
            dt.pop_back();
        }
        return dt;
    }

    void logEvent(const string& username, const string& action) {
        ofstream file(logFile, ios::app);
        if (file.is_open()) {
            file << "[" << getCurrentTime() << "] User: " << username << " | Action: " << action << "\n";
            file.close();
        }
    }

    string hashString(const string& input) {
        hash<string> hasher;
        return to_string(hasher(input + "SALT_KEY_V3_2026"));
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
        ifstream file(userFile);
        User u;
        while (file >> u.username >> u.passwordHash >> u.email >> u.role >> u.securityAnswerHash) {
            users.push_back(u);
        }
        file.close();
        return users;
    }

    void saveAllUsers(const vector<User>& users) {
        ofstream file(userFile, ios::trunc);
        for (const auto& u : users) {
            file << u.username << " " << u.passwordHash << " " << u.email << " " << u.role << " " << u.securityAnswerHash << "\n";
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
        string username, password, email, roleChoice, role, secAnswer;
        cout << "\n--- User Registration (v3.0) ---\n";
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
            if (isStrongPassword(password)) break;
            cout << "Weak password. Please meet the requirements.\n";
        }

        cout << "Security Question - What is your favorite city? (no spaces): ";
        cin >> secAnswer;

        cout << "Select Role (1 for User, 2 for Admin): ";
        cin >> roleChoice;
        role = (roleChoice == "2") ? "ADMIN" : "USER";

        ofstream file(userFile, ios::app);
        if (file.is_open()) {
            file << username << " " << hashString(password) << " " << email << " " << role << " " << hashString(secAnswer) << "\n";
            file.close();
            logEvent(username, "REGISTERED (" + role + ")");
            cout << "Registration successful! Account created with role: " << role << "\n";
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

            if (users[userIndex].passwordHash == hashString(password)) {
                logEvent(username, "LOGIN_SUCCESS");
                cout << "Login successful! Welcome, " << username << " [" << users[userIndex].role << "].\n";
                if (users[userIndex].role == "ADMIN") {
                    adminDashboard(username);
                } else {
                    userDashboard(username);
                }
                return;
            } else {
                attempts++;
                logEvent(username, "LOGIN_FAILED_ATTEMPT_" + to_string(attempts));
                cout << "Incorrect password. Attempts remaining: " << (maxAttempts - attempts) << "\n";
            }
        }

        logEvent(username, "ACCOUNT_LOCKED");
        cout << "Account temporarily locked due to 3 failed attempts.\n";
    }

    void forgotPassword() {
        string username, secAnswer, newPass;
        cout << "\n--- Password Recovery ---\n";
        cout << "Enter Username: ";
        cin >> username;

        vector<User> users = loadUsers();
        for (auto& u : users) {
            if (u.username == username) {
                cout << "Security Question - What is your favorite city?: ";
                cin >> secAnswer;

                if (u.securityAnswerHash != hashString(secAnswer)) {
                    logEvent(username, "PASSWORD_RECOVERY_FAILED");
                    cout << "Error: Incorrect security answer.\n";
                    return;
                }

                while (true) {
                    cout << "Enter New Password (min 6 chars, 1 letter & 1 number): ";
                    cin >> newPass;
                    if (isStrongPassword(newPass)) break;
                    cout << "Weak password. Try again.\n";
                }

                u.passwordHash = hashString(newPass);
                saveAllUsers(users);
                logEvent(username, "PASSWORD_RESET_VIA_RECOVERY");
                cout << "Password reset successful! You can now log in.\n";
                return;
            }
        }
        cout << "Error: Username not found.\n";
    }

    void changePassword(const string& username) {
        string oldPass, newPass;
        cout << "\n--- Change Password ---\n";
        cout << "Enter Current Password: ";
        cin >> oldPass;

        vector<User> users = loadUsers();
        for (auto& u : users) {
            if (u.username == username) {
                if (u.passwordHash != hashString(oldPass)) {
                    cout << "Error: Current password is incorrect.\n";
                    return;
                }

                while (true) {
                    cout << "Enter New Password (min 6 chars, 1 letter & 1 number): ";
                    cin >> newPass;
                    if (isStrongPassword(newPass)) break;
                    cout << "Weak password. Try again.\n";
                }

                u.passwordHash = hashString(newPass);
                saveAllUsers(users);
                logEvent(username, "PASSWORD_CHANGED");
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
        logEvent(username, "ACCOUNT_DELETED");
        cout << "Account deleted permanently.\n";
    }

    void viewAllUsers() {
        vector<User> users = loadUsers();
        cout << "\n--- Registered Users List ---\n";
        for (const auto& u : users) {
            cout << "Username: " << u.username << " | Email: " << u.email << " | Role: " << u.role << "\n";
        }
    }

    void viewActivityLogs() {
        ifstream file(logFile);
        string line;
        cout << "\n--- System Activity Logs ---\n";
        if (!file.is_open()) {
            cout << "No logs found.\n";
            return;
        }
        while (getline(file, line)) {
            cout << line << "\n";
        }
        file.close();
    }

    void adminDashboard(const string& username) {
        int choice;
        while (true) {
            cout << "\n=== Admin Dashboard (" << username << ") ===\n";
            cout << "1. View All Registered Users\n";
            cout << "2. View System Activity Logs\n";
            cout << "3. Change Password\n";
            cout << "4. Logout\n";
            cout << "Choose an option: ";

            if (!(cin >> choice)) {
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Invalid input.\n";
                continue;
            }

            if (choice == 1) {
                viewAllUsers();
            } else if (choice == 2) {
                viewActivityLogs();
            } else if (choice == 3) {
                changePassword(username);
            } else if (choice == 4) {
                logEvent(username, "LOGOUT");
                cout << "Logged out successfully.\n";
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        }
    }

    void userDashboard(const string& username) {
        int choice;
        while (true) {
            vector<User> users = loadUsers();
            string currentEmail = "";
            for (const auto& u : users) {
                if (u.username == username) currentEmail = u.email;
            }

            cout << "\n=== User Dashboard (" << username << ") ===\n";
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
                cout << "Role:     USER\n";
            } else if (choice == 2) {
                changePassword(username);
            } else if (choice == 3) {
                deleteAccount(username);
                break;
            } else if (choice == 4) {
                logEvent(username, "LOGOUT");
                cout << "Logged out successfully.\n";
                break;
            } else {
                cout << "Invalid choice.\n";
            }
        }
    }
};

int main() {
    AuthSystem auth;
    int choice;

    while (true) {
        cout << "\n====================================\n";
        cout << " Login & Registration System (v3.0)\n";
        cout << "====================================\n";
        cout << "1. Register\n";
        cout << "2. Login\n";
        cout << "3. Forgot Password\n";
        cout << "4. Exit\n";
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
                auth.forgotPassword();
                break;
            case 4:
                cout << "Exiting program.\n";
                return 0;
            default:
                cout << "Invalid option. Please try again.\n";
        }
    }
}
