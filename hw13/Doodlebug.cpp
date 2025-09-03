#include <iostream>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <string>


using namespace std;

vector<int> getOrder();

class Organism;

class Game {
public:
    Game(int maxNum = 400, int timeCount = 0, int dbCount = 5, int antCount = 100); //initial values to set the gameboard
    Game(const Game& rhs);
    Game& operator=(const Game& rhs);
    ~Game();

    void setBoard();
    void displayBoard();
    void playGame();

private:
    int maxNum;
    Organism** arr; //creates a gameboard of Organism pointer type
    int timeCount;
    int dbCount;
    int antCount;

};

class Organism {
public:
    Organism(int pos, int time = 0, bool flag = false) : position(pos), timestep(time), moved(flag) {}
    Organism(const Organism& rhs) : position(rhs.position), timestep(rhs.timestep), moved(rhs.moved) {}
    Organism& operator=(const Organism& rhs) {
        if (this != &rhs) { position = rhs.position; timestep = rhs.timestep; moved = rhs.moved; }
        return *this;
    }
    virtual ~Organism() {}

    //functions to identify organism type, move on the board, and breed new organisms
    virtual char Speak() const = 0; //pure virtual function, makes Organism an abstract class
    virtual void Move(Organism** arr) = 0;
    virtual void Breed(Organism** arr) = 0;
    virtual int getStepsSinceEat() const { return 0; }
    virtual void addStepsSinceEat() {} 
    virtual void resetStepsSinceEat() {}
    virtual bool eatAnts(Organism** arr) { return false; } //function checks if Ants in random adjacent cell, and if so eats the Ant

    //getters
    int getPos() const { return position; }
    int getTime() const { return timestep; }
    bool getMoved() const { return moved; }

    //setters
    void setPos(int p) { position = p; }
    void setTime(int t) { timestep = t; }
    void setMoved(bool b) { moved = b; }

    //function increments time for every World step that organism is alive
    void addTime() { timestep = timestep + 1; }

private:
    int position;
    int timestep;
    bool moved;

};

class Ant : public Organism {
public:
    Ant(int pos) : Organism(pos) {}
    Ant(const Ant& rhs) : Organism(rhs) {}
    Ant& operator=(const Ant& rhs) {
        if (this != &rhs) { Organism::operator=(rhs); }
        return *this;
    }
    virtual ~Ant() {}

    virtual char Speak() const { return 'o'; } //meant to identify an Ant
    virtual void Move(Organism** arr);
    virtual void Breed(Organism** arr);

};

class Doodlebug : public Organism {
public:
    Doodlebug(int pos) : Organism(pos), stepsSinceEat(0) {}
    Doodlebug(const Doodlebug& rhs) : Organism(rhs), stepsSinceEat(rhs.stepsSinceEat) {}
    Doodlebug& operator=(const Doodlebug& rhs) {
        if (this != &rhs) { Organism::operator=(rhs); stepsSinceEat = rhs.stepsSinceEat; }
        return *this;
    }
    virtual ~Doodlebug() {}

    virtual char Speak() const { return 'X'; } //meant to identify a Doodlebug
    virtual void Move(Organism** arr);
    virtual void Breed(Organism** arr);
    virtual int getStepsSinceEat() const { return stepsSinceEat; } //gets time since last ate an ant
    virtual void addStepsSinceEat() { stepsSinceEat++; } //enter steps since ate ants
    virtual void resetStepsSinceEat() { stepsSinceEat = 0; }
    virtual bool eatAnts(Organism** arr); //function checks if Ants in random adjacent cell, and if so eats the Ant
    

private:
    int stepsSinceEat;
};



int main() {
    srand((time(NULL)));

    Game TestGame;
    TestGame.setBoard();
    TestGame.displayBoard();
    TestGame.playGame();

}

void Game::setBoard() {
    //placing Doodlebugs randomly on the Board
    while (dbCount > 0) {
        int randNum = rand() % maxNum;
        if (arr[randNum] == nullptr) {
            arr[randNum] = new Doodlebug(randNum);
            dbCount--;
        }
    }
    //placing Ants randomly on the Board
    while (antCount > 0) {
        int randNum = rand() % maxNum;
        if (arr[randNum] == nullptr || (*arr[randNum]).Speak() != 'X') {
            arr[randNum] = new Ant(randNum);
            antCount--;
        }
    }
}

void Game::displayBoard() {
    //outputting the Board
    cout << "World at time " << timeCount << ":" << endl << endl;
    for (int i = 0; i < maxNum; i++) {
        if (arr[i] != nullptr) {
            cout << (*arr[i]).Speak();
        }
        else {
            cout << "-";
        }
        cout << " ";
        if ((i + 1) % 20 == 0) {
            cout << endl;
        }
    }
}

void Game::playGame() {
    string userInput; //created to detect if a user is clicking "enter"

    while (true) {
        cout << endl << "Press ENTER to continue";
        getline(cin, userInput);

        //resets setMoved value for all live Organisms
        for (int i = 0; i < maxNum; i++) {
            if (arr[i] != nullptr) {
                (*arr[i]).setMoved(false);
            }
        }

        //goes through each Doodlebug and causes it to Move, if it hasn't already
        if (userInput.empty()) { //mechanism to wait for user Input to continue
            for (int i = 0; i < maxNum; i++) {
                if (arr[i] != nullptr) {
                    if ((*arr[i]).Speak() == 'X' && (*arr[i]).getMoved() == false) {
                        (*arr[i]).Move(arr);
                    }
                }
            }

            //this is checking whether Doodlebug should be starved
            for (int i = 0; i < maxNum; i++) {
                if (arr[i] != nullptr) {
                    if ((*arr[i]).Speak() == 'X' && (*arr[i]).getStepsSinceEat() > 2) {
                        delete arr[i];
                        arr[i] = nullptr;
                        continue;
                    }
                }
            }

            //this checks whether Doodlebug should breed
            for (int i = 0; i < maxNum; i++) {
                if (arr[i] != nullptr) {
                    if ((*arr[i]).Speak() == 'X' && (*arr[i]).getTime() > 0 && (*arr[i]).getTime() % 8 == 0) {
                        (*arr[i]).Breed(arr);
                    }
                }
            }

            //this makes Ants move 
            for (int i = 0; i < maxNum; i++) {
                if (arr[i] != nullptr) {
                    if ((*arr[i]).Speak() == 'o' && (*arr[i]).getMoved() == false) {
                        (*arr[i]).Move(arr);
                    }
                }
            }
            //this checks whether Ants should breed
            for (int i = 0; i < maxNum; i++) {
                if (arr[i] != nullptr) {
                    if ((*arr[i]).Speak() == 'o' && (*arr[i]).getTime() > 0 && (*arr[i]).getTime() % 3 == 0 && (*arr[i]).getMoved() == true) {
                        (*arr[i]).Breed(arr);
                    }
                }
            }

            timeCount++;
            displayBoard();
        }
        else {
            break;
        }
    }
}


//Sets up a vector of the 4 directions in random order
vector<int> getOrder() {
    int numDirections = 4;
    int rand1, rand2, rand3, rand4, randCall;

    rand1 = rand() % numDirections;
    randCall = rand1;

    while (randCall == rand1) {
        randCall = rand() % numDirections;
        if (randCall != rand1) {
            rand2 = randCall;
            break;
        }
    }
    while (randCall == rand2 || randCall == rand1) {
        randCall = rand() % numDirections;
        if (randCall != rand2 && randCall != rand1) {
            rand3 = randCall;
            break;
        }
    }
    while (randCall == rand3 || randCall == rand2 || randCall == rand1) {
        randCall = rand() % numDirections;
        if (randCall != rand3 && randCall != rand2 && randCall != rand1) {
            rand4 = randCall;
            break;
        }
    }
    vector<int> directionOrder = { rand1, rand2, rand3, rand4 };
    return directionOrder;
}


//Move function to determine how each Ant should move
void Ant::Move(Organism** arr) {
    int numDirections = 4;
    int randPos = rand() % numDirections;
    const int left = 0, right = 1, up = 2, down = 3;
    int temp = getPos();


    if (randPos == left) {
        if (arr[temp - 1] == nullptr && temp != 0 && temp % 20 != 0) {
            arr[temp - 1] = this;
            arr[temp] = nullptr;
            setPos(temp - 1);
            setMoved(true);
        }
    }
    else if (randPos == right) {
        if (arr[temp + 1] == nullptr && (temp + 1) % 20 != 0) {
            arr[temp + 1] = this;
            arr[temp] = nullptr;
            setPos(temp + 1);
            setMoved(true);
        }
    }
    else if (randPos == up) {
        if (arr[temp - 20] == nullptr && temp > 19) {
            arr[temp - 20] = this;
            arr[temp] = nullptr;
            setPos(temp - 20);
            setMoved(true);
        }
    }
    else if (randPos == down) {
        if (arr[temp + 20] == nullptr && temp < 380) {
            arr[temp + 20] = this;
            arr[temp] = nullptr;
            setPos(temp + 20);
            setMoved(true);
        }
    }
    setMoved(true);
    addTime();
}


//instructions for how each Ant should Breed

void Ant::Breed(Organism** arr) {
    int temp = getPos();
    vector<int> dirOrder = getOrder();
    const int left = 0, right = 1, up = 2, down = 3;

    for (int i : dirOrder) {
        if (i == left && arr[temp - 1] == nullptr && temp != 0 && temp % 20 != 0) {
            arr[temp - 1] = new Ant(temp - 1);
            break;
        }
        else if (i == right && arr[temp + 1] == nullptr && (temp + 1) % 20 != 0) {
            arr[temp + 1] = new Ant(temp + 1);
            break;
        }
        else if (i == up && arr[temp - 20] == nullptr && temp > 19) {
            arr[temp - 20] = new Ant(temp - 20);
            break;
        }
        else if (i == down && arr[temp + 20] == nullptr && temp < 380) {
            arr[temp + 20] = new Ant(temp + 20);
            break;
        }
    }
}

//Move function to determine how each Doodlebug should move
void Doodlebug::Move(Organism** arr) {
    int numDirections = 4;
    int randPos = rand() % numDirections;
    const int left = 0, right = 1, up = 2, down = 3;
    int temp = getPos();

    bool eatCheck = eatAnts(arr);

    if (eatCheck) {
        resetStepsSinceEat();
    }

    if (!eatCheck) {
        addStepsSinceEat();
        if (randPos == left) {
            if (arr[temp - 1] == nullptr && temp != 0 && temp % 20 != 0) {
                arr[temp - 1] = this;
                arr[temp] = nullptr;
                setPos(temp - 1);
            }
        }
        else if (randPos == right) {
            if (arr[temp + 1] == nullptr && (temp + 1) % 20 != 0) {
                arr[temp + 1] = this;
                arr[temp] = nullptr;
                setPos(temp + 1);
            }
        }
        else if (randPos == up) {
            if (arr[temp - 20] == nullptr && temp > 19) {
                arr[temp - 20] = this;
                arr[temp] = nullptr;
                setPos(temp - 20);
            }
        }
        else if (randPos == down) {
            if (arr[temp + 20] == nullptr && temp < 380) {
                arr[temp + 20] = this;
                arr[temp] = nullptr;
                setPos(temp + 20);
            }
        }
        setMoved(true);
    }
    addTime();

}

//instructions for how each Doodlebug should Breed
void Doodlebug::Breed(Organism** arr) {
    int temp = getPos();
    vector<int> dirOrder = getOrder();
    const int left = 0, right = 1, up = 2, down = 3;

    for (int i : dirOrder) {
        if (i == left && arr[temp - 1] == nullptr && temp != 0 && temp % 20 != 0) {
            arr[temp - 1] = new Doodlebug(temp - 1);
            break;
        }
        else if (i == right && arr[temp + 1] == nullptr && (temp + 1) % 20 != 0) {
            arr[temp + 1] = new Doodlebug(temp + 1);
            break;
        }
        else if (i == up && arr[temp - 20] == nullptr && temp > 19) {
            arr[temp - 20] = new Doodlebug(temp - 20);
            break;
        }
        else if (i == down && arr[temp + 20] == nullptr && temp < 380) {
            arr[temp + 20] = new Doodlebug(temp + 20);
            break;
        }
    }
}

//instructions for how each Doodlebug should eat an Ant
bool Doodlebug::eatAnts(Organism** arr) {
    vector<int> dirOrder = getOrder();
    const int left = 0, right = 1, up = 2, down = 3;
    int temp = getPos();


    for (int i : dirOrder) {
        if (i == left && arr[temp - 1] != nullptr && temp > 0) {
            if ((*arr[temp - 1]).Speak() == 'o') {
                delete arr[temp - 1];
                arr[temp - 1] = this;
                arr[temp] = nullptr;
                setPos(temp - 1);
                setMoved(true);
                return true;
            }
        }
        else if (i == right && arr[temp + 1] != nullptr && temp < 399) {
            if ((*arr[temp + 1]).Speak() == 'o') {
                delete arr[temp + 1];
                arr[temp + 1] = this;
                arr[temp] = nullptr;
                setPos(temp + 1);
                setMoved(true);
                return true;
            }
        }
        else if (i == up && arr[temp - 20] != nullptr && temp > 19) {
            if ((*arr[temp - 20]).Speak() == 'o') {
                delete arr[temp - 20];
                arr[temp - 20] = this;
                arr[temp] = nullptr;
                setPos(temp - 20);
                setMoved(true);
                return true;
            }
        }
        else if (i == down && arr[temp + 20] != nullptr && temp < 380) {
            if ((*arr[temp + 20]).Speak() == 'o') {
                delete arr[temp + 20];
                arr[temp + 20] = this;
                arr[temp] = nullptr;
                setPos(temp + 20);
                setMoved(true);
                return true;
            }
        }
    }
    return false;
}

Game::Game(int maxNum, int timeCount, int dbCount, int antCount) : maxNum(maxNum), timeCount(timeCount), dbCount(dbCount), antCount(antCount) {
    arr = new Organism * [maxNum];
    for (int i = 0; i < maxNum; i++) {
        arr[i] = nullptr;
    }
}

Game::Game(const Game& rhs) : maxNum(rhs.maxNum), timeCount(rhs.timeCount), dbCount(rhs.dbCount), antCount(rhs.antCount) {
    arr = new Organism * [maxNum];
    for (int i = 0; i < maxNum; i++) {
        arr[i] = rhs.arr[i]; //deep copy of elements
    }
}

Game& Game::operator=(const Game& rhs) {
    if (this != &rhs) {
        maxNum = rhs.maxNum;
        timeCount = rhs.timeCount;
        dbCount = rhs.dbCount;
        antCount = rhs.antCount;
        for (int i = 0; i < maxNum; i++) {
            if (arr[i] != nullptr) {
                delete arr[i];
                arr[i] = nullptr;
            }
        }
        delete[] arr;
        arr = nullptr;
        arr = new Organism * [maxNum];
        for (int i = 0; i < maxNum; i++) {
            arr[i] = rhs.arr[i];
        }
        return *this;
    }
}

Game::~Game() {
    //cleans up dynamically allocated memory
    for (int i = 0; i < maxNum; i++) {
        if (arr[i] != nullptr) {
            delete arr[i];
            arr[i] = nullptr;
        }
    }
    delete[] arr;
}
