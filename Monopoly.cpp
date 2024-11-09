#include <iostream>
#include <vector>
#include <string>
#include <fstream>  // For file I/O
#include <sstream>  // For string stream
using namespace std;

class Property {
public:
    int price;
    int rent;
    int houseRent;
    int mortgage;
    int owner; 

    /* Constructor for Property attributes
    INCLUDES: 
    Price, Rent, fullSetRent, 1-4 houseRent, hotelRent, mortgage 
    */
    Property(int price, int rent, int houseRent, int mortgage)
        : price(price), rent(rent), houseRent(houseRent), mortgage(mortgage), owner(-1) {}

    // Function to display property details
    void displayProperty() const {
        cout << "Price: $" << price << " | Rent: $" << rent
            << " | House Rent: $" << houseRent << " | Mortgage: $" << mortgage;
        if (owner != -1) {
            cout << " | Owned by Player ID: " << owner;  // Display owner ID if exists
        }
    }
};

class BoardSpace {
public:
    string name;
    string type;
    Property* propertyDetails;  // Pointer to Property object, nullptr if not a property

    // Constructor for non-property spaces
    BoardSpace(string name, string type)
        : name(name), type(type), propertyDetails(nullptr) {}

    // Constructor for property spaces
    BoardSpace(string name, string type, int price, int rent, int houseRent, int mortgage)
        : name(name), type(type) {
        propertyDetails = new Property(price, rent, houseRent, mortgage);
    }

    // Copy constructor
    BoardSpace(const BoardSpace& other)
        : name(other.name), type(other.type) {
        if (other.propertyDetails) {
            propertyDetails = new Property(*other.propertyDetails);  // Deep copy
        }
        else {
            propertyDetails = nullptr;
        }
    }

    // Copy assignment operator
    BoardSpace& operator=(const BoardSpace& other) {
        if (this != &other) {  // Self-assignment check
            delete propertyDetails;  // Free existing memory

            name = other.name;
            type = other.type;
            if (other.propertyDetails) {
                propertyDetails = new Property(*other.propertyDetails);  // Deep copy
            }
            else {
                propertyDetails = nullptr;
            }
        }
        return *this;
    }

    // Destructor to free memory
    ~BoardSpace() {
        delete propertyDetails;
    }

    // Function to display space details
    void displaySpace() const {
        cout << "Tile: " << name << " | Type: " << type;
        if (propertyDetails) {
            cout << " | ";
            propertyDetails->displayProperty();
        }
        cout << endl;
    }
};

class Board {
    // Class for the entire board
public:
    //A vector that will hold objects of each space and their details inside 
    vector<BoardSpace> spaces;

    // Constructor that loads from a file 
    Board(const string& filename) {
        loadBoardFromFile(filename);
    }

    // Function to load the board from a file
    void loadBoardFromFile(const string& filename) {
        ifstream file(filename);
        if (!file.is_open()) {
            cout << "Error opening file!" << endl;
            return;
        }
        //[Line]: Variable store an entire file line 
        string line;
        //The loop will continously collect each file line until there are none left. For each line collected, it will determine what type of space it is and collect the proper details accordingly. 
        while (getline(file, line)) {
            stringstream attribute(line);
            string name, type, priceStr, rentStr, houseRentStr, mortgageStr;
            int price = 0, rent = 0, houseRent = 0, mortgage = 0;

            //Collecting the basic attributes on every space 
            getline(attribute, name, ',');
            getline(attribute, type, ',');
            
            //Adds special types when a property is collected 
            if (type == "Property") {
                if (getline(attribute, priceStr, ',') && !priceStr.empty() &&
                    getline(attribute, rentStr, ',') && !rentStr.empty() &&
                    getline(attribute, houseRentStr, ',') && !houseRentStr.empty() &&
                    getline(attribute, mortgageStr, ',') && !mortgageStr.empty()) {
                    

                    // Convert strings from the collected stream into integers
                    try {
                        price = stoi(priceStr);
                        rent = stoi(rentStr);
                        houseRent = stoi(houseRentStr);
                        mortgage = stoi(mortgageStr);
                    }
                    catch (const invalid_argument&) {
                        cerr << "Invalid number format in file for: " << name << endl;
                        continue;  // Skip this entry
                    }
                }
                // Add property space to the vector
                spaces.push_back(BoardSpace(name, type, price, rent, houseRent, mortgage));
            }
            else {
                // Create BoardSpace without property details
                spaces.push_back(BoardSpace(name, type));
            }
        }
        file.close();
    }

    // Function to display the board
    void displayBoard() const {
        for (size_t i = 0; i < spaces.size(); ++i) {
            spaces[i].displaySpace(); 
        }
    }
};

class Player {
    int position;
    int balance; 
    int id;
    vector<BoardSpace*> ownedProperties;
    bool inJail = false; 
    vector<BoardSpace>& spaces;

public:
    Player(int playerID, vector<BoardSpace>& gameSpaces) : id(playerID), position(0), balance(1500), spaces(gameSpaces) {}

    int move(int steps) {
        position = (position + steps) % spaces.size();

        //Determines which space has been landed on 
        BoardSpace& landedSpace = spaces[position];

        //Interaction when a player lands on a property 
        if (landedSpace.type == "Property") {
            if (landedSpace.propertyDetails) {

                //Grabs details for the property 
                Property* property = landedSpace.propertyDetails;

                //Interaction for unknown properties 
                if (property->owner == -1) {
                    cout << "Would you like to buy " << landedSpace.name << " for $" << property->price << "? Y/N";
                    char input;
                    cin >> input;
                    if (input == 'Y' || input == 'y') {
                        if (balance >= property->price) {
                            property->owner = id;
                            ownedProperties.push_back(&landedSpace);
                            balance -= property->price;
                            cout << "\nYou have purchased " << landedSpace.name << "." << endl;
                        }
                        else {
                            cout << "You cannot afford " << landedSpace.name << "." << endl;
                        }
                    }
                    else if (input == 'N' || input == 'n') {
                        cout << "You did not purchase " << landedSpace.name << "." << endl;
                    }
                    else {
                        cout << "Invlid Input, enter either Y or N." << endl;
                    }
                }

                //Interaction for properties owned by other players 
                else if (property->owner != id && property->owner != -1) {
                    cout << "This property is owned by Player" << property->owner << endl;
                    // Implement rent logic if necessary
                    //Take away player money 
                    balance -= property->rent;
                    //Give owner player the same amount 
                }

                //Interaction for properties owned by the current player 
                else {
                    cout << "You own " << landedSpace.name << "." << endl;
                }
            }
           
        }

        //Interaction when a player lands on a Community Chest 
        else if (landedSpace.type == "Community Chest") {

        }

        //Interaction when a player lands on a Chance 
        else if (landedSpace.type == "Chance") {

        }

        //Interaction when a player lands on Jail 
        else if (landedSpace.type == "Jail") {

        }
        
        //Interaction when a player lands on GO
        else if (landedSpace.type == "GO") {
            cout << "Passed GO, collect $200." << endl;
            balance += 200;
        }

        //Interaction whene a player lands on Free Parking 
        else if (landedSpace.type == "Free Parking") {

        }
        return position; 
    }
};

int main() {
    // Main function
    Board monopolyBoard("board.txt");  // Load board data from the file
    monopolyBoard.displayBoard();      // Show the board
    return 0;
}
