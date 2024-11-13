#include <iostream>
#include <vector>
#include <string>
#include <fstream>  
#include <sstream>  
#include <random> 
using namespace std;

/*This is what each Property will be classified as, and where it will construct the details
  [displayProperty()] : A way to see all details of a chosen property outputted
*/
class Property {
public:
    int price;
    int rent;
    vector <int> houseRent;
    int mortgage;
    int owner;
    bool isMortgaged;


    /* Constructor for Property attributes
    INCLUDES:
    [price]: The Price of the Property
    [rent]: The Base Rent of the Property
    [houseRent]: A list of the Rent Prices for every house 1-4 + hotel
    [mortgage]: The amount given when the property is mortgaged

    INCLUDED BUT NOT IN FILE:
    [owner]: The mark for whoever owns the property (-1 means unowned)
    [isMortgaged] The status of whether or not the property is mortgaged
    */
    Property(int price, int rent, vector <int> houseRent, int mortgage, bool isMortgaged)
        : price(price), rent(rent), houseRent(houseRent), mortgage(mortgage), owner(-1), isMortgaged(false) {}

    // Function to display property details
    void displayProperty() const {
        cout << "Price: $" << price << " | Rent: $" << rent << " | House Rents: ";
        for (int rentLevel : houseRent) {
            cout << "$" << rentLevel << " ";
        }
        cout << "| Mortgage: $" << mortgage;
        if (owner != -1) {
            cout << " | Owned by Player: " << owner;
        }
        cout << endl;
    }
};

/*This defines each individual board space, that will read what type it is and create a constructor accordingly
  [displaySpace()] : A way to see the name and type of space you land on
*/
class BoardSpace {
public:
    string name;
    string type;
    // Pointer to Property object, null if not a property
    Property* propertyDetails;  

    // Constructor for non-property spaces
    BoardSpace(string name, string type)
        : name(name), type(type), propertyDetails(nullptr) {}

    // Constructor for property spaces
    BoardSpace(string name, string type, int price, int rent, vector <int> houseRent, int mortgage)
        : name(name), type(type) {
        propertyDetails = new Property(price, rent, houseRent, mortgage, false);
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

/*This will load the files holding all extra details and put them in the constructors made in the BoardSpace and Property classes
  [loadBoardFromFile()] : Will open a file and read each detail and assign them one by one
  [displayBoard() : Will display all spaces
*/
class Board {
    // Class for the entire board
public:
    //A vector that opens the board.txt file and will hold objects of each space and their details inside 
    vector<BoardSpace> spaces;
    Board(const string& filename) {
        loadBoardFromFile(filename);  // Load the board data from the file
    }
    void loadBoardFromFile(const string& filename) {
        ifstream file(filename);
        //Notice for uncesessful file opening 
        if (!file.is_open()) {
            cout << "Error opening file!" << endl;
            return;
        }
        string line;
        while (getline(file, line)) {
            //Using streams, the program can read each part of the file and assign them to the proper varaibles 
            stringstream attribute(line);
            string name, type, priceStr, rentStr, mortgageStr, colorStr;
            string houseRentStr1, houseRentStr2, houseRentStr3, houseRentStr4, houseRentStr5;
            vector<int> houseRentLevels;  // Vector to store multiple house rent levels
            int price = 0, rent = 0, mortgage = 0, houseCost = 0;

            //Obtains the Name and type that are used on all spaces
            getline(attribute, name, ',');
            getline(attribute, type, ',');

            //Spaces with the Property type will also grab the information about price, rent, house prices, and mortgage values
            if (type == "Property") {
                if (getline(attribute, priceStr, ',') && !priceStr.empty() &&
                    getline(attribute, rentStr, ',') && !rentStr.empty()) {
                    //Attempts to collect price and rent values 
                    try {
                        price = stoi(priceStr);
                        rent = stoi(rentStr);
                    }
                    //Detects any possible errors and proceeds when one happens 
                    catch (const invalid_argument&) {
                        cout << "Invalid number file for: " << name << endl;
                        continue;
                    }
                    // Collects 5 house rent values
                    if (getline(attribute, houseRentStr1, ',') && !houseRentStr1.empty()) {
                        houseRentLevels.push_back(stoi(houseRentStr1));
                    }
                    if (getline(attribute, houseRentStr2, ',') && !houseRentStr2.empty()) {
                        houseRentLevels.push_back(stoi(houseRentStr2));
                    }
                    if (getline(attribute, houseRentStr3, ',') && !houseRentStr3.empty()) {
                        houseRentLevels.push_back(stoi(houseRentStr3));
                    }
                    if (getline(attribute, houseRentStr4, ',') && !houseRentStr4.empty()) {
                        houseRentLevels.push_back(stoi(houseRentStr4));
                    }
                    if (getline(attribute, houseRentStr5, ',') && !houseRentStr5.empty()) {
                        houseRentLevels.push_back(stoi(houseRentStr5));
                    }
                    // Collects the mortgage value
                    if (getline(attribute, mortgageStr, ',')) {
                        try {
                            mortgage = stoi(mortgageStr);
                        }
                        catch (const invalid_argument&) {
                            cout << "Invalid mortgage file for: " << name << endl;
                            mortgage = 0; 
                        }
                    }
                }
                // Fills the spaces vector will all the details of a Property Space 
                spaces.push_back(BoardSpace(name, type, price, rent, houseRentLevels, mortgage));
            }


            else {
                // Fills the spaces vector with all the details of a non-Property Space
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

/*This will be the actions and setup of the players
  [rollDice()] : This handles the player whenever they roll the dice, both while in jail and not in jail [rollForDoubles()] and [payToGetOut()] are helper functions for this function 
  [checkDouble()] : Compares both rolled dice to see if they match 
  [displayRoll()] : Outputs each dice result 
  [displayBalance()] : Shows a player's money 
  [displayInventory()] : This will allow a Player to look at all of their currently owned properties
  [landOnJail()] : Applies the jail status to the player and resets the amount of turns they have been in Jail
  [getIsInJail()] : Returns a Player's Jail status 
  [sell()] : This will let you mortage or remove houses from a property of your choosing
  [bankruptCheck()] : Returns a player's bankrupt status 
  [move()] : This will cause the player to move on the board and trigger all interactions that happen when a player would land on any type of space
*/
class Player {
    int position;
    int balance;
    int id;
    int dice1, dice2;
    vector<BoardSpace*> ownedProperties;
    vector<Player*>& players;
    bool inJail = false;
    bool isBankrupt = false; 
    int turnsInJail;
    vector<BoardSpace>& spaces;
    const int collectCard = 1;
    const int fineCard = 2;
    const int advanceCard = 3;
    const int jailCard = 4;

public:
    Player(int playerID, vector<BoardSpace>& gameSpaces, vector<Player*>& eachPlayer) : id(playerID), position(0), balance(1500), spaces(gameSpaces), players(eachPlayer), dice1(0), dice2(0), inJail(false), turnsInJail(0), isBankrupt(false) {}
    struct Card {
        int effect;
        int amount;
        int targetSpace;
    };
    int rollDice() {
        if (inJail == false) {
            dice1 = (rand() % 6) + 1;
            dice2 = (rand() % 6) + 1;
            int rollTotal = dice1 + dice2;
            cout << "Total: " << rollTotal << endl;
            return rollTotal;
        }
        else {
            int input=0;
            cout << "Would you like to pay to get out(1), or roll for doubles(2)" << endl;
            while (input != 1 && input != 2) {
                cin >> input;
                if (input == 2) {
                    rollForDoubles(dice1, dice2); 
                }
                else {
                    payToGetOut();
                }
            }
        }
    }
    bool checkDouble() {
        return dice1 == dice2;
    }
    void displayRoll() {
        cout << "Dice 1: " << dice1 << endl << "Dice 2: " << dice2 << endl;
    }
    void displayBalance() {
        cout << "Current Balance: " << balance << endl;
    }
    void displayInventory() {
        if (ownedProperties.empty()) {
            cout << "You do not own any properties." << endl;
            return;
        }
        else {
            cout << "Your owned properties: " << endl;
            for (int i = 0; i < ownedProperties.size(); i++) {
                BoardSpace* space = ownedProperties[i];
                cout << i + 1 << ". " << space->name << " (Mortgage Value: $" << space->propertyDetails->mortgage << ")" << endl;
            }
        }
    }
    void landOnJail() {
        inJail = true;
        turnsInJail = 0;
        cout << "You are now in Jail!" << endl;
    }
    void rollForDoubles(int roll1, int roll2) {
        if (roll1 == roll2) {
            inJail = false;
            cout << "You rolled doubles and are free from Jail!" << endl;
        }
        else {
            turnsInJail++;
            if (turnsInJail >= 3) {
                cout << "You've been in Jail for 3 turns. You can pay a fine to get out!" << endl;
            }
        }
    }
    void payToGetOut() {
        if (balance >= 50) {  // Example: Jail fine cost
            balance -= 50;
            inJail = false;
            cout << "You paid $50 to get out of Jail!" << endl;
        }
        else {
            cout << "You don't have enough money to pay the fine!" << endl;
        }
    }
    bool getIsInJail() const {
        return inJail;
    }
    void sell(Property& sellingProperty) {
        //Checks to see if you own the property
        if (sellingProperty.owner == id) {
            //Adds the mortgage value to your account 
            balance += sellingProperty.mortgage;
        }
        else {
            cout << "This Property is now mortgaged." << endl;
        }
    }
    bool bankruptCheck() {
        if (balance <= 0) {
            isBankrupt = true; 
        }
        return isBankrupt; 
    }
    bool buyProperty(BoardSpace& landedSpace, Property* property, int& balance, int id, vector<BoardSpace*>& ownedProperties) {
        if (balance >= property->price) {
            cout << "Would you like to buy " << landedSpace.name << " for $" << property->price << "? Y/N" << endl;
            char input;
            bool correctInput = false;

            // Loops until a proper input is given
            while (!correctInput) {
                cin >> input;

                // Choose to purchase the property
                if (input == 'Y' || input == 'y') {
                    property->owner = id;
                    ownedProperties.push_back(&landedSpace);
                    balance -= property->price;
                    cout << "You have purchased " << landedSpace.name << "." << endl;
                    return true; // Indicate purchase was made
                }
                // Choose not to purchase the property
                else if (input == 'N' || input == 'n') {
                    cout << "You did not purchase " << landedSpace.name << "." << endl;
                    return false; // Indicate no purchase was made
                }
                else {
                    cout << "Invalid input, enter either Y or N." << endl;
                }
            }
        }
        else {
            // Chose to purchase but cannot afford it
            cout << "You cannot afford " << landedSpace.name << "." << endl;
        }

        return false; // Indicate no purchase was made if balance insufficient
    }
    void ChanceChest(Player& player, const vector<Card>& cardDeck) {
        int randomIndex = rand() % cardDeck.size();
        Card drawnCard = cardDeck[randomIndex];

        if (drawnCard.effect == collectCard) {
            player.balance += drawnCard.amount;
            cout << "You collected $" << drawnCard.amount << "!" << endl;
        }
        else if (drawnCard.effect == fineCard) {
            player.balance -= drawnCard.amount;
            cout << "You paid a fine of $" << drawnCard.amount << "." << endl;
        }
        else if (drawnCard.effect == advanceCard) {
            player.position = drawnCard.targetSpace;
            cout << "You moved to space " << drawnCard.targetSpace << "." << endl;
        }
        else if (drawnCard.effect == jailCard) {
            player.move(drawnCard.amount);  // Assumes move method advances player by given spaces
            cout << "You advanced " << drawnCard.amount << " spaces." << endl;
        }
       
         
    }


    int move(int steps) {
        //Ensures player cannot move to a space beyond the board size
        position = (position + steps) % spaces.size();

        //Determines which space has been landed on 
        BoardSpace& landedSpace = spaces[position];

        //Interaction when a player lands on a property 
        if (landedSpace.type == "Property") {
            if (landedSpace.propertyDetails) {

                //Grabs details for the property 
                Property* property = landedSpace.propertyDetails;
                cout << "You landed on " << landedSpace.name << endl;
                //Interaction for unknown properties 
                if (property->owner == -1) {
                    bool purchased = buyProperty(landedSpace, property, balance, id, ownedProperties);
                }


                //Interaction for properties owned by other players 
                else if (property->owner != id && property->owner != -1) {
                    cout << "This property is owned by Player " << property->owner +1 << endl;
                    //Checks if player can afford rent
                    if (balance >= property->rent) {
                        //Takes away rent from player 
                        balance -= property->rent;
                        //Adds the rent to the owner 
                        players[property->owner]->balance += property->rent;
                    }
                    //If player cannot pay rent
                    else {
                        cout << "Not enough money to pay rent" << endl;
                        //Force player to pay rent (SELL FUNCTION) 
                        int choice;
                        bool soldItem = false;
                        while (soldItem == false && balance < property->rent) {
                            //Inventory method that displays all properties and current balance 
                            cout << "Enter the Property you want to sell, 0 to open your inventory, or -1 to declare bankruptcy" << endl;
                            cin >> choice;
                            //Declare Banruptcy 
                            if (choice == -1) {
                                cout << "You have declared bankruptcy" << endl;
                                isBankrupt = true; 
                                break;
                            }
                            //Open Inventory 
                            if (choice == 0) {
                                displayInventory();
                            }
                            //Choose a property to sell (Displays all owned properties the user has)
                            if (choice > 0 && choice <= ownedProperties.size()) {
                                BoardSpace* selectedSpace = ownedProperties[choice - 1];
                                if (selectedSpace && selectedSpace->propertyDetails) {
                                    Property* sellingChoice = selectedSpace->propertyDetails;
                                    sell(*sellingChoice);
                                    soldItem = true;
                                }
                                else {
                                    cout << "Invalid option" << endl;
                                }
                            }
                        }
                    }
                }
                //Interaction for properties owned by the current player 
                else {
                    cout << "You own " << landedSpace.name << "." << endl;
                }
            }
        }


        //Interaction when a player lands on a Community Chest 
        else if (landedSpace.type == "Chest" || landedSpace.type == "Chance") {
            cout << "You landed on " << landedSpace.name << endl;
            vector<Card> cardDeck = {
       {collectCard, 100, 0},
       {fineCard, 50, 0},
       {advanceCard, 0, 10},  // Go to space 10
       {jailCard, 3, 0}  // Move forward by 3 spaces
            };
            ChanceChest(*this, cardDeck);
        }

        //Interaction when a player lands on Jail 
        else if (landedSpace.type == "Jail") {
            cout << "You landed on " << landedSpace.name << endl;
            // Sending the player to Jail
            landOnJail(); // Call the landOnJail method to set inJail and reset the counter
            position = 10;
            cout << "You are sent directly to Jail!" << endl;
        }

        //Interaction when a player lands on GO 
        else if (landedSpace.type == "GO") {
            cout << "You landed on " << landedSpace.name << endl;
            cout << "Passed GO, collect $200." << endl;
            balance += 200;
        }

        //Interaction whene a player lands on Free Parking (Nothing will Happen)
        else if (landedSpace.type == "Free Parking" || landedSpace.type == "Visiting") {
            if (landedSpace.type == "Free Parking") {
                cout << "Landed on Free Parking " << endl;
            }
            else {
                cout << "Landed on Just Visting " << endl;
            }
            cout << "Nothing Happens." << endl;
        }

        else if (landedSpace.type == "Tax") {
            balance -= 100;
            cout << "Landed on Tax, balance is now " << balance; 
            
        }
        return position;
    }
};

int main() {
    Board monopolyBoard("board.txt");  // Load board data from the file
    //monopolyBoard.displayBoard();       TEST Show the board
    srand(static_cast<unsigned int>(time(0)));  // Seed random number generator
    int input; 

    // Create vector to store players and pass it to each Player object
    vector<Player*> players;
    int turn = 0;
    int turnNum = 0;
    // Create a few players with player IDs 0, 1, and 2
    Player player1(0, monopolyBoard.spaces, players);
    Player player2(1, monopolyBoard.spaces, players);
    Player player3(2, monopolyBoard.spaces, players);
    Player player4(3, monopolyBoard.spaces, players);

    cout << "How many players? (1-4)" << endl;
    cin >> input;
    // Add players to the vector for referencing by other players
    if (input >= 1) players.push_back(&player1);
    if (input >= 2) players.push_back(&player2);
    if (input >= 3) players.push_back(&player3);
    if (input == 4) players.push_back(&player4);

    cout << "Number of players: " << players.size() << endl;

    //Gameplay Loop
    while (player1.bankruptCheck() == false && player2.bankruptCheck() == false && player3.bankruptCheck() == false && player4.bankruptCheck() == false) {
        Player* currentPlayer = players[turn];
        cout << "Turn: " << turnNum << endl;
        cout << "PLayer " << turn + 1 << "'s turn. Roll the Dice with R or Check your Inventory with I" << endl;
        char choice = ' ';
        while (choice != 'R' && choice != 'r') {
            cin >> choice; 
            if (choice == 'I' || choice == 'i') {
                currentPlayer->displayBalance();
                currentPlayer->displayInventory();
            }
            if (choice == 'R' || choice == 'r') {
                int rollTotal = 0;
                int rollCount = 0;
                bool isDouble = true;
                while (isDouble == true && rollCount < 3) {
                    rollTotal = currentPlayer->rollDice();
                    currentPlayer->displayRoll();
                    isDouble = currentPlayer->checkDouble();
                    rollCount++;
                    if (rollCount != 3) {
                        currentPlayer->move(rollTotal);
                    }
                    if (isDouble) {
                        cout << "Doubles, rolling again " << endl;
                        currentPlayer->move(rollTotal);
                    }
                    else {
                        cout << "No doubles " << endl;
                        isDouble = false; 
                    }
                    if (rollCount == 3 && isDouble) {
                        cout << "Three Doubles, sending to Jail" << endl;
                        currentPlayer->landOnJail(); 
                        isDouble = false; 
                    }
                }
            }
           
        }
        turn = (turn + 1) % players.size();
        turnNum++;
    }
    return 0;
}


