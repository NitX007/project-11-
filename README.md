# project-11-
simple login and registration system


# Login & Registration System (C++)

A console-based user authentication and account management system built in C++ using file handling and object-oriented programming.

## Features

### Version 1.0 (MVP)
- User registration with duplicate username check
- User login authentication
- Persistent storage using local text files
- Basic user dashboard with logout option

### Version 2.0 (Current)
- **Password Hashing:** Stores salted password hashes instead of plain text
- **Password Validation:** Enforces minimum 6 characters with at least one letter and one number
- **Brute-Force Protection:** Limits login to 3 failed attempts
- **Account Management:** View profile details (username & email), change password, and delete account

 ### Version 3.0 (Current)
- **Role-Based Access Control (RBAC):** Supports separate `USER` and `ADMIN` roles with dedicated dashboards
- **Forgot Password Recovery:** Allows users to reset forgotten passwords by verifying a hashed security question answer
- **Audit Logging:** Automatically records timestamped login, registration, failed attempt, and password change events in `activity.log`
- **Admin Controls:** Admins can view all registered users and inspect system activity logs
