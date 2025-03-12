#include <iostream>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <string>
#include <limits>

using namespace std;


// Book Class
class Book {
    private:
        string title;
        string author;
        string ISBN;
        bool available;
    
    public:
        Book(string title, string author, string ISBN)
            : title(title), author(author), ISBN(ISBN), available(true) {}
    
        string getTitle() const { return title; }
        string getAuthor() const { return author; }
        string getISBN() const { return ISBN; }
        bool isAvailable() const { return available; }
    
        void borrowBook() { available = false; }
        void returnBook() { available = true; }
    
        void displayBook() const {
            cout << "Title: " << title << ", Author: " << author
                 << ", ISBN: " << ISBN << ", Available: " << (available ? "Yes" : "No") << endl;
        }
    };


// User Class
class User {
    protected:
        string name;
        int userID;
        vector<string> borrowedBooks; // Stores ISBNs of borrowed books
        static int nextID;
    
    public:
        User(string name) : name(name) {
            userID = nextID++;
        }

        virtual ~User() {} // destructor to esnure clean up
    
        virtual string getUserType() const { return "User"; }
        int getUserID() const { return userID; }
        string getName() const { return name; }
    
        void borrowBook(const string& ISBN) { borrowedBooks.push_back(ISBN); }
        void returnBook(const string& ISBN) {
            if (borrowedBooks.empty()) {
                cout << "Error: No borrowed books to return.\n";
                return;
            }
        
            auto it = std::remove(borrowedBooks.begin(), borrowedBooks.end(), ISBN);
            
            if (it == borrowedBooks.end()) {
                cout << "Error: Book with ISBN " << ISBN << " not found in borrowed books.\n";
                return;
            }
        
            borrowedBooks.erase(it, borrowedBooks.end()); // ensure erase
        }
    
        void displayUser() const {
            cout << getUserType() << " - Name: " << name << ", ID: " << userID << ", Borrowed Books: ";
            for (const auto& isbn : borrowedBooks) cout << isbn << " ";
            cout << endl;
        }
    };
    
    int User::nextID = 1; // Initialize static user ID counter


// Student Class: Inheriting from User
class Student : public User {
    public:
        Student(string name) : User(name) {}
    
        string getUserType() const override { return "Student"; }
    };


// Faculty Class: Inheriting from User
class Faculty : public User {
    public:
        Faculty(string name) : User(name) {}
    
        string getUserType() const override { return "Faculty"; }
    };


// Library Class
class Library {
    private:
        vector<Book> books;
        unordered_map<int, User*> users; // Maps user ID to User object

        Library() {} // Private constructor to prevent external instantiation

        // Delete copy constructor and assignment operator to prevent copies
        Library(const Library&) = delete;
        Library& operator=(const Library&) = delete;
    
    public:
        static Library& getInstance() {
            static Library instance; // single instance of Library
            return instance;
        } 
        
        ~Library() {
            for (auto& pair : users) {
                delete pair.second; // Free dynamically allocated memory
            }
        }
    
        void addBook(const string& title, const string& author, const string& ISBN) {
            books.emplace_back(title, author, ISBN);
        }
    
        void addUser(User* user) {
            users[user->getUserID()] = user;
        }
    
        void borrowBook(int userID, const string& ISBN) {
            auto userIt = users.find(userID);
            if (userIt == users.end()) { // handle error is user does not exist
                cout << "Error: User not found.\n";
                return;
            }

            for (auto& book : books) {
                if (book.getISBN() == ISBN) {
                    if (!book.isAvailable()) { // handle error if the book is already being borrowed
                        cout << "Error: Book \"" << book.getTitle() << "\" is already borrowed.\n";
                        return;
                    }
                    book.borrowBook();
                    userIt->second->borrowBook(ISBN);
                    cout << userIt->second->getName() << " borrowed \"" << book.getTitle() << "\".\n";
                    return;
                }
            }

            cout << "Error: Book with ISBN " << ISBN << " not found.\n"; // if the book is not found
        }
    
        void returnBook(int userID, const string& ISBN) {
            auto userIt = users.find(userID);
            if (userIt == users.end()) {
                cout << "Error: User not found.\n"; // if user not found
                return;
            }
        
            for (auto& book : books) {
                if (book.getISBN() == ISBN) {
                    if (book.isAvailable()) {
                        cout << "Error: Book \"" << book.getTitle() << "\" was never borrowed.\n"; // if the user never borrowed the book
                        return;
                    }
                    book.returnBook();
                    userIt->second->returnBook(ISBN);
                    cout << userIt->second->getName() << " returned \"" << book.getTitle() << "\".\n";
                    return;
                }
            }
        
            cout << "Error: Book with ISBN " << ISBN << " not found.\n"; // book doesn't exist
        }
        
    
        void displayBooks() const {
            cout << "\nLibrary Books:\n";
            for (const auto& book : books) {
                book.displayBook();
            }
        }
        

        // searches for books and prints message if no books are found
        void searchBooks(const string& query) const {
            cout << "\nSearch Results for '" << query << "':\n";
            bool found = false;
        
            for (const auto& book : books) {
                if (book.getTitle().find(query) != string::npos || book.getAuthor().find(query) != string::npos) {
                    book.displayBook();
                    found = true;
                }
            }
        
            if (!found) {
                cout << "No books found matching the query.\n";
            }
        }
        

        void displayUsers() const {
            cout << "\nLibrary Users:\n";
            for (const auto& pair : users) {
                pair.second->displayUser();
            }
        }
    };


// runs the User interaction interface
// gives user 9 option choices within a "home menu"
// handles invalid option choices
void runCLI() {
    Library& library = Library::getInstance();
    int choice;
    while (true) {
        cout << "\n===== Library System =====\n";
        cout << "1. Add a new book\n";
        cout << "2. Register a new user\n";
        cout << "3. Search for books\n";
        cout << "4. Borrow a book\n";
        cout << "5. Return a book\n";
        cout << "6. Display books\n";
        cout << "7. Display users\n";
        cout << "8. Exit\n";
        cout << "Choose an option: ";
        cin >> choice;
        
        if (cin.fail()) {  // handle invalid input (letters instead of numbers)
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n'); 
            cout << "Invalid input! Enter a number between 1-8.\n";
            continue;
        }
        
        cin.ignore();
        if (choice == 8) { // exit program if user selects exit choice
            cout << "Goodbye!\n";
            break;
        }
    
        string title, author, ISBN, name;
        int userID;
    
        switch (choice) {
            case 1:
                cout << "Enter book title: ";
                getline(cin, title);
                cout << "Enter author: ";
                getline(cin, author);
                cout << "Enter ISBN: ";
                getline(cin, ISBN);
                library.addBook(title, author, ISBN);
                cout << "Book added successfully!\n";
                break;
    
            case 2:
                cout << "Enter user name: ";
                getline(cin, name);
                cout << "Register as: (1) Student (2) Faculty: ";
                int userType;
                cin >> userType;
                cin.ignore();

                if (cin.fail() || (userType != 1 && userType != 2)) {  // ensure correct user type input
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid selection. Please enter 1 for Student or 2 for Faculty.\n";
                    break;
                }

                if (userType == 1) {
                    library.addUser(new Student(name));
                } else {
                    library.addUser(new Faculty(name));
                }
                cout << "User registered successfully!\n";
                break;

            case 3:
                cout << "Enter book title or author to search: ";
                getline(cin, title);
                library.searchBooks(title);
                break;
    
            case 4:
                cout << "Enter user ID: ";
                cin >> userID;

                if (cin.fail()) {  // handle invalid user ID input
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input! Please enter a valid user ID.\n";
                    break;
                }
                
                cin.ignore();
                cout << "Enter book ISBN to borrow: ";
                getline(cin, ISBN);
                library.borrowBook(userID, ISBN);
                break;
    
            case 5:
                cout << "Enter user ID: ";
                cin >> userID;

                if (cin.fail()) {  // handle invalid user ID input
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    cout << "Invalid input! Please enter a valid user ID.\n";
                    break;
                }

                cin.ignore();
                cout << "Enter book ISBN to return: ";
                getline(cin, ISBN);
                library.returnBook(userID, ISBN);
                break;
    
            case 6:
                library.displayBooks();
                break;
    
            case 7:
                library.displayUsers();
                break;
    
            default:
                cout << "Invalid option! Please try again.\n"; // redirect users if invalid range was selected
        }
    }
}    


// Main Function
int main() {
    runCLI();
    return 0;
}