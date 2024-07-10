#include <iostream>
#include <fstream>
#include <string>
#include <iomanip>
#include <limits>
#include <chrono> // for time-based operations
#include <ctime>  // for time functions

using namespace std;

struct User {
    string username;
    string password;
};

struct UserNode {
    User user;
    UserNode* next;

    UserNode(const string& username, const string& password) : user{username, password}, next(nullptr) {}
};

class UserLinkedList {
private:
    UserNode* head;

public:
    UserLinkedList() : head(nullptr) {}

    ~UserLinkedList() {
        UserNode* current = head;
        while (current != nullptr) {
            UserNode* next = current->next;
            delete current;
            current = next;
        }
    }

    void addUser(const string& username, const string& password) {
        UserNode* newNode = new UserNode(username, password);
        newNode->next = head;
        head = newNode;
    }

    bool authenticateUser(const string& username, const string& password) const {
        UserNode* current = head;
        while (current != nullptr) {
            if (current->user.username == username && current->user.password == password) {
                return true;
            }
            current = current->next;
        }
        return false;
    }

    void loadUsersFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening " << filename << "!" << endl;
            return;
        }

        string username, password;
        while (file >> username >> password) {
            addUser(username, password);
        }

        file.close();
    }
};

struct Movie {
    string name;
    float price;
};

void displayMovieList() {
    cout << endl;
    cout << "MOVIE LIST\n";
    cout << "1. Matrix - $10.00\n";
    cout << "2. Blade Runner - $12.00\n";
    cout << "3. Brothers - $8.00\n";
    cout << "4. God Father - $15.00\n";
    cout << "5. Wolf Of Wallstreets - $11.50\n";
    cout << "6. The Boys - $9.00\n";
    cout << endl;
}

void displayTimeSlots() {
    cout << endl;
    cout << "TIME SLOTS\n";
    cout << "1. Morning Show (10:00 AM)\n";
    cout << "2. Afternoon Show (2:00 PM)\n";
    cout << "3. Evening Show (6:00 PM)\n";
    cout << "4. Night Show (9:00 PM)\n";
    cout << endl;
}

class SeatTree {
private:
    struct ReservationNode {
        int seatNumber;
        bool reserved;
        std::chrono::time_point<std::chrono::system_clock> reservationTime; // Corrected line
        ReservationNode* left;
        ReservationNode* right;

        ReservationNode(int number) : seatNumber(number), reserved(false), left(nullptr), right(nullptr) {
            reservationTime = std::chrono::system_clock::now();
        }
    };

    ReservationNode* root;

    ReservationNode* insertRecursive(ReservationNode* node, int number) {
        if (node == nullptr) {
            return new ReservationNode(number);
        }

        if (number < node->seatNumber) {
            node->left = insertRecursive(node->left, number);
        } else if (number > node->seatNumber) {
            node->right = insertRecursive(node->right, number);
        }

        return node;
    }

    void destroyTree(ReservationNode* node) {
        if (node) {
            destroyTree(node->left);
            destroyTree(node->right);
            delete node;
        }
    }

public:
    SeatTree() : root(nullptr) {}

    ~SeatTree() {
        destroyTree(root);
    }

    void insert(int number) {
        root = insertRecursive(root, number);
    }

    bool reserve(int number) {
        ReservationNode* current = root;
        while (current) {
            if (current->seatNumber == number) {
                // Check if the seat is already reserved
                if (!current->reserved) {
                    // If the seat is not reserved, mark it as reserved and record the reservation time
                    current->reserved = true;
                    current->reservationTime = std::chrono::system_clock::now();
                    return true; // Reservation successful
                } else {
                    // Seat already reserved
                    return false;
                }
            } else if (number < current->seatNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return false; // Seat not found
    }

    bool isReserved(int number) {
        ReservationNode* current = root;
        while (current) {
            if (current->seatNumber == number) {
                // Check if the seat is reserved
                if (current->reserved) {
                    // Check if the reservation has expired (more than 12 hours)
                    auto currentTime = std::chrono::system_clock::now();
                    auto elapsedTime = std::chrono::duration_cast<std::chrono::hours>(currentTime - current->reservationTime);
                    if (elapsedTime.count() > 12) {
                        // If the reservation has expired, remove it
                        current->reserved = false;
                        return false;
                    }
                    // Reservation is
                    // Reservation is still valid
                    return true;
                } else {
                    // Seat is not reserved
                    return false;
                }
            } else if (number < current->seatNumber) {
                current = current->left;
            } else {
                current = current->right;
            }
        }
        return false; // Seat not found
    }
};

const int MAX_SEATS = 50;
SeatTree seatTree;

void initializeSeats() {
    for (int i = 1; i <= MAX_SEATS; ++i) {
        seatTree.insert(i);
    }
}

void displaySeats() {
    cout << "Seats: ";
    for (int i = 1; i <= MAX_SEATS; ++i) {
        cout << (seatTree.isReserved(i) ? 'X' : 'O') << " ";
    }
    cout << endl;
}

void reserveSeats(const string& movieName, int numSeats) {
    cout << "\nSEAT RESERVATION FOR " << movieName << endl;
    displaySeats();

    int availableSeats = 0;
    for (int i = 1; i <= MAX_SEATS; ++i) {
        if (!seatTree.isReserved(i)) {
            ++availableSeats;
        }
    }

    if (availableSeats < numSeats) {
        cout << "Not enough available seats. Reservation failed." << endl;
        return;
    }

    int reservedSeats = 0;
    for (int i = 1; i <= MAX_SEATS; ++i) {
        if (!seatTree.isReserved(i)) {
            seatTree.reserve(i);
            ++reservedSeats;
            if (reservedSeats == numSeats) {
                break;
            }
        }
    }

    cout << "Seats reserved successfully." << endl;
    displaySeats();
}

float calculateTotalCost(int numSeats, float pricePerSeat) {
    return numSeats * pricePerSeat;
}

void displayReceipt(const User& user, const Movie& movie, const string& timeSlot, int numSeats, float totalCost) {
    cout << endl;
    cout << "\n\nRECEIPT\n";
    cout << "Customer: " << user.username << endl;
    cout << "Movie: " << movie.name << endl;
    cout << "Time Slot: " << timeSlot << endl;
    cout << "Number of Seats: " << numSeats << endl;
    cout << "Total Cost: $" << fixed << setprecision(2) << totalCost << endl;
    cout << "Thank you for booking with us!\n";
    cout << endl;
}

int main() {
    string username, password;

    UserLinkedList userList;
    userList.loadUsersFromFile("user_details.txt");

    cout << "Enter username: ";
    cin >> username;

    cout << "Enter password: ";
    cin >> password;

    if (userList.authenticateUser(username, password)) {
        cout << "Authentication successful! Welcome, " << username << "." << endl;
    } else {
        cout << "Authentication failed. Invalid username or password." << endl;
        return 1;
    }

    initializeSeats();

    int mainMenuChoice;
    int movieChoice, timeSlotChoice, numSeats;
    Movie selectedMovie;
    string timeSlot;
    float totalCost = 0.0;

    while (true) {
        cout << "\nMAIN MENU\n";
        cout << "1. Select Movie List\n";
        cout << "2. Exit Program\n";
        cout << "Enter your choice: ";
        cin >> mainMenuChoice;

        switch (mainMenuChoice) {
            case 1:
                displayMovieList();

                cout << "Enter the number corresponding to the movie you want to watch: ";
                cin >> movieChoice;

                if (cin.fail() || movieChoice < 1 || movieChoice > 6) {
                    cout << "Invalid choice. Exiting the program.\n";
                    return 1;
                }

                switch (movieChoice) {
                    case 1:
                        selectedMovie = {"Matrix", 10.00};
                        break;
                    case 2:
                        selectedMovie = {"Blade Runner", 12.00};
                        break;
                    case 3:
                        selectedMovie = {"Brothers", 8.00};
                        break;
                    case 4:
                        selectedMovie = {"God Father", 15.00};
                        break;
                    case 5:
                        selectedMovie = {"Wolf Of Wallstreets", 11.50};
                        break;
                    case 6:
                        selectedMovie = {"The Boys", 9.00};
                        break;
                    default:
                        cout << "Invalid choice. Exiting the program.\n";
                        return 1;
                }

                displayTimeSlots();
                cout << "Enter the number corresponding to the time slot you prefer: ";
                cin >> timeSlotChoice;

                if (cin.fail() || timeSlotChoice < 1 || timeSlotChoice > 4) {
                    cout << "Invalid choice. Exiting the program.\n";
                    return 1;
                }

                switch (timeSlotChoice) {
                    case 1:
                        timeSlot = "Morning Show (10:00 AM)";
                        break;
                    case 2:
                        timeSlot = "Afternoon Show (2:00 PM)";
                        break;
                    case 3:
                        timeSlot = "Evening Show (6:00 PM)";
                        break;
                    case 4:
                        timeSlot = "Night Show (9:00 PM)";
                        break;
                    default:
                        cout << "Invalid choice. Exiting the program.\n";
                        return 1;
                }

                cout << "Enter the number of seats you want to reserve: ";
                cin >> numSeats;

                if (cin.fail() || numSeats <= 0) {
                    cout << "Invalid input. Please enter a valid number of seats." << endl;
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    break;
                }

                reserveSeats(selectedMovie.name, numSeats);
                totalCost = calculateTotalCost(numSeats, selectedMovie.price);
                displayReceipt({username, password}, selectedMovie, timeSlot, numSeats, totalCost);

                break;
            case 2:
                cout << "Exiting the program. Goodbye!\n";
                return 0;
            default:
                cout << "Invalid choice. Please enter a valid option.\n";
                break;
        }
    }

    return 0;
}

