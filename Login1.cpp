#include <iostream>
#include <fstream>
#include <string>
#include <stack>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <regex>
using namespace std;

// Structure for Student Node (Linked List)
struct Student {
    string rollno, name, email, password;
    Student* next;

    Student(string r, string n, string e, string p) {
        rollno = r;
        name = n;
        email = e;
        password = p;
        next = NULL;
    }
};

Student* head = NULL; // Head pointer for linked list`
stack<string> loginHistory; // Stack to track last logins

// Function to insert student node into linked list
void insertStudent(string r, string n, string e, string p) {
    Student* newNode = new Student(r, n, e, p);
    if (head == NULL) {
        head = newNode;
    } else {
        Student* temp = head;
        while (temp->next != NULL)
            temp = temp->next;
        temp->next = newNode;
    }
}

// Function to save all linked list data to file
void saveToFile() {
    ofstream file("students.txt");
    Student* temp = head;
    while (temp != NULL) {
        file << temp->rollno << "," << temp->name << ","
             << temp->email << "," << temp->password << endl;
        temp = temp->next;
    }
    file.close();
}

// Function to load data from file into linked list
void loadFromFile() {
    ifstream file("students.txt");
    if (!file) {
        cout << "No existing student data found. Starting fresh.\n";
        return;
    }

    string line;
    while (getline(file, line)) {
        if (line.empty()) continue;
        string r, n, e, p;
        int pos1 = line.find(",");
        int pos2 = line.find(",", pos1 + 1);
        int pos3 = line.find(",", pos2 + 1);

        r = line.substr(0, pos1);
        n = line.substr(pos1 + 1, pos2 - pos1 - 1);
        e = line.substr(pos2 + 1, pos3 - pos2 - 1);
        p = line.substr(pos3 + 1);

        insertStudent(r, n, e, p);
    }
    file.close();
}

// Roll number validation (exactly 10 characters)
bool isValidRollNo(const string& roll) {
    return roll.size() == 10;
}

// Email format validation
bool isValidEmail(const string& email){
    const regex pattern(R"(^[a-zA-Z0-9._%+-]+@[a-zA-Z0-9.-]+\.[a-zA-Z]{2,}$)");
    return regex_match(email,pattern);
}
// bool isValidEmail(const string& email) {
//     int at = email.find('@');
//     int dot = email.find('.', at);
//     if (at == string::npos || dot == string::npos) return false;
//     if (at == 0 || dot == email.size() - 1) return false;
//     if (dot < at) return false;
//     return true;
// }

// Password format validation (min 8 chars, has uppercase, lowercase, digit, special char)
bool isValidPassword(const string& password) {
    if (password.length() < 8) return false;
    bool hasUpper = false, hasLower = false, hasDigit = false, hasSpecial = false;
    for (char ch : password) {
        if (isupper(ch)) hasUpper = true;
        else if (islower(ch)) hasLower = true;
        else if (isdigit(ch)) hasDigit = true;
        else hasSpecial = true;
    }
    return hasUpper && hasLower && hasDigit && hasSpecial;
}

// Function to register a new student with all validations
void registerStudent() {
    string r, n, e, p;
    cout << "\n---- Student Registration ----\n";

    // Roll No validation
    while (true) {
        cout << "Enter Roll No (10 characters, e.g., RBT24CB032): ";
        cin >> r;
        if (isValidRollNo(r))
            break;
        else
            cout << "Invalid Roll No! Must be exactly 10 characters.\n";
    }

    cout << "Enter Name(First Name and Last name): ";
    cin.ignore();
    getline(cin, n);

    // Email validation
    while (true) {
        cout << "Enter Email (e.g., name@gmail.com): ";
        cin >> e;
        if (isValidEmail(e))
            break;
        else
            cout << "Invalid Email Format! Try again.\n";
    }

    // Password validation
    while (true) {
        cout << "Enter Password (min 8 chars, include A-Z, a-z, 0-9, symbol): ";
        cin >> p;
        if (isValidPassword(p))
            break;
        else
            cout << "Weak Password! Must include uppercase, lowercase, number & symbol.\n";
    }

    // Check if email already exists
    Student* temp = head;
    while (temp != NULL) {
        if (temp->email == e) {
            cout << "\n⚠  Email already registered!\n";
            return;
        }
        temp = temp->next;
    }

    insertStudent(r, n, e, p);
    saveToFile();
    cout << "\n Registration successful!\n";
}

// Function to search for student (login)
Student* searchStudent(string email, string password) {
    Student* temp = head;
    while (temp != NULL) {
        if (temp->email == email && temp->password == password)
            return temp;
        temp = temp->next;
    }
    return NULL;
}

// Function to login a student
void loginStudent() {
    string email, pass;
    cout << "\n---- Student Login ----\n";
    cout << "Enter Email: ";
    cin >> email;
    cout << "Enter Password: ";
    cin >> pass;

    Student* s = searchStudent(email, pass);
    if (s != NULL) {
        cout << "\nLogin successful!\n";
        cout << "\n---- Student Profile ----\n";
        cout << left << setw(15) << "Roll No" << ": " << s->rollno << endl;
        cout << left << setw(15) << "Name" << ": " << s->name << endl;
        cout << left << setw(15) << "Email" << ": " << s->email << endl;

        loginHistory.push(s->email);
    } else {
        cout << "\n Invalid email or password!\n";
    }
}

// Function to view all students (sorted by name)
void viewAllStudents() {
    cout << "\n---- All Registered Students ----\n";
    vector<pair<string, string>> students;
    Student* temp = head;
    while (temp != NULL) {
        students.push_back({temp->name, temp->email});
        temp = temp->next;
    }

    if (students.empty()) {
        cout << "No records found.\n";
        return;
    }

    sort(students.begin(), students.end());

    cout << left << setw(20) << "Name" << setw(30) << "Email" << endl;
    cout << "------------------------------------------------------\n";
    for (auto s : students)
        cout << left << setw(20) << s.first << setw(30) << s.second << endl;
}

// Function to show last few logged-in users
void showLoginHistory() {
    cout << "\n---- Recent Logins ----\n";
    if (loginHistory.empty()) {
        cout << "No recent logins.\n";
        return;
    }

    stack<string> tempStack = loginHistory;
    int count = 1;
    while (!tempStack.empty() && count <= 5) {
        cout << count << ". " << tempStack.top() << endl;
        tempStack.pop();
        count++;
    }
}

// Function to delete a student by email
void deleteStudent() {
    string email;
    cout << "\nEnter the email of the student to delete: ";
    cin >> email;

    Student* temp = head;
    Student* prev = NULL;

    while (temp != NULL && temp->email != email) {
        prev = temp;
        temp = temp->next;
    }

    if (temp == NULL) {
        cout << "No student found with that email.\n";
        return;
    }

    if (prev == NULL)
        head = temp->next;
    else
        prev->next = temp->next;

    delete temp;
    saveToFile();
    cout << "Student deleted successfully.\n";
}

// Main menu
int main() {
    loadFromFile();
    int choice;

    while (true) {
        cout << "\n===== Student Platform =====\n";
        cout << "1. Register Student\n";
        cout << "2. Login Student\n";
        cout << "3. View All Students\n";
        cout << "4. Show Recent Logins\n";
        cout << "5. Delete a Student\n";
        cout << "6. Exit\n";
        cout << "Enter choice: ";
        cin >> choice;

        switch (choice) {
        case 1:
            registerStudent();
            break;
        case 2:
            loginStudent();
            break;
        case 3:
            viewAllStudents();
            break;
        case 4:
            showLoginHistory();
            break;
        case 5:
            deleteStudent();
            break;
        case 6:
            cout << "\nExiting... Thank you!\n";
            saveToFile();
            return 0;
        default:
            cout << "\nInvalid choice. Try again!\n";
        }
    }
}