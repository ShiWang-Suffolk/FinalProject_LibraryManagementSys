
# 📚 Library Book Tracking System

This is a C++ console-based library management system with SQLite integration. It allows users to search, borrow, and return books. Administrators can add, delete, or modify book records and register users.

## ✅ Features

### 👤 User Roles
- **Admin**
  - Add/Edit/Delete books
  - Register new users
- **Student**
  - Search/View books
  - Borrow and return books
  - View borrowing history
  - Check overdue status

## 🛠 Technologies Used
- C++17
- SQLite3
- Makefile build system
- Tested on Windows + Eclipse CDT & WSL

## 🔧 Installation

### 1. Prerequisites
- C++ compiler (`g++`)
- SQLite3 development library

For WSL:
```bash
sudo apt install g++ sqlite3 libsqlite3-dev
```

For Windows:
- Make sure `sqlite3.dll`, `sqlite3.lib`, and `sqlite3.h` are placed in a shared path (e.g. `C:\Users\YourName\sqlite3`)
- Add that path to your `INCLUDE`, `LIB`, and `Path` environment variables

### 2. Build the Project
```bash
make
```

### 3. Run the Program
```bash
./ls
```

## 👥 Default Users (for testing)

| Role    | Username  | Password |
|---------|-----------|----------|
| Admin   | admin     | 1234     |
| Student | student1  | 1234     |

## 📂 Project Structure

```
.
├── sources/
│   ├── main.cpp
│   ├── core.cpp
│   ├── ui.cpp
│   └── sqlite3.c
├── headers/
│   ├── core.h
│   ├── ui.h
│   └── sqlite3.h
├── library.db
├── Makefile
└── README.md
```

## 💡 Notes

- All user data is stored in `library.db`
- Admin and student roles are distinguished by the `role` column in the `users` table
- SQLite is used via `sqlite3.c` and `sqlite3.h` directly compiled into the project

## 🚀 License

This project is for academic use only.
