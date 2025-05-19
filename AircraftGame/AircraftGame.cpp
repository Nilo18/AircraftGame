#include <iostream>
#include <vector>
#include <windows.h>
#include <random>
#include <fstream>
using namespace std;
bool gameIsRunning = true; 
int score = 0;
void endGame();
void deleteMissile();
void deleteObstacle(int index);
void gameLoop();
inline void setCursorPosition(int x, int y);

// Function for displaying OR updating the score
inline void showCurrentScore(int add = 0) {
    if (add != 0) score += add; 
    setCursorPosition(55, 0);
    cout << "Score: " << score << endl;
}

// Function for hiding the cursor inside the console
inline void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); // Get handle to the console
    CONSOLE_CURSOR_INFO cursorInfo; // Initialize the class containing cursor info
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;  // Make the cursor disappear
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

// Get handle to the console
HANDLE getConsole() {
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

// Function for getting the right most edge of the console
int getConsoleRightmostX() {
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;
    //HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

    // Add some safety
    if (GetConsoleScreenBufferInfo(getConsole(), &screenBufferInfo)) {
        return screenBufferInfo.srWindow.Right;
    }

    return -1; // Return -1 if there's an error
}

inline int getConsoleBottomMostY() {
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

    if (GetConsoleScreenBufferInfo(getConsole(), &screenBufferInfo)) {
        return screenBufferInfo.srWindow.Bottom;
    }

    return -1; // Return -1 if there's an error
}

// Function for setting cursor position inside the console
inline void setCursorPosition(int x, int y) { 
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(getConsole(), coord);
}

// Function for clearing the previous frame
void clearFrame(int x, int y, int rows, int cols) { 
    // Clear the frame according to the given size
    for (int i = 0; i < rows; i++) {
        setCursorPosition(x, y + i);
        for (int j = 0; j < cols; j++) {
            cout << " "; 
        }
    }
}

// Function for displaying a new frame
void displayFrame(int x, int y, vector<vector<int>> objectToDisplay, int gamespeed = 10) {
    for (int i = 0; i < objectToDisplay.size(); i++) {
        setCursorPosition(x, y + i);
        for (int j = 0; j < objectToDisplay[i].size(); j++) {
            int cell = objectToDisplay[i][j];

            if (cell == 0) {
                cout << ' ';
                continue;
            }

            // Set red color if this is the top-point '*' (represented by 11)
            if (cell == 11 && i == 0) {
                SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | FOREGROUND_INTENSITY);
                cout << '*';
            }
            else if (cell == 11 && i == 2) {
                SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                cout << '*';
            }
            else {
                switch (cell) {
                case 10: cout << '/'; break;
                case 110: cout << '\\'; break;
                case 111:
                    SetConsoleTextAttribute(
                        getConsole(),
                        FOREGROUND_GREEN | FOREGROUND_INTENSITY |
                        BACKGROUND_GREEN | BACKGROUND_INTENSITY
                    );
                    cout << ' '; 
                    break;

                default: cout << '|'; break;
                }
            }

            // Reset color back to white (default)
            SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }

    // Sleep(gamespeed); // Optional for animation delay
}


class Obstacle {
private:
    vector<vector<int>> obstacleRep = {
        {111, 111},
        {111, 111}
    };
    int startX;
    int startY = 1;
    int bottomLimit = 25;
    const int rows = 2;
    const int cols = 2;
public:
    Obstacle();
    ~Obstacle() = default;
    void moveDown();
    int getX() const { return startX; };
    int getY() const { return startY; };
    int getRows() const { return rows; };
    int getCols() const { return cols; };
    int getBottomLimit() const { return bottomLimit; };
};

Obstacle::Obstacle() {
    random_device rd;
    default_random_engine dre(rd());
    uniform_int_distribution <int> dis(2, getConsoleRightmostX() - 2);

    this->startX = dis(dre);
    displayFrame(startX, startY, obstacleRep);
}

void Obstacle::moveDown() {
    clearFrame(startX, startY, rows, cols); // Pass the size of the obstacle as well as the coordinates DONE!
    if (startY < bottomLimit) startY++;
    displayFrame(startX, startY, obstacleRep);
}

vector<Obstacle*> activeObstacles; // Vector for counting active obstacles

// Define a missile class
class Missile {
private:
    vector <vector<int>> missileRep = {
        {10, 110},
        {1, 1},
        {11, 11}
    };
    int startX;
    int startY = 22;
    const int rows = 3;
    const int cols = 2;
public:
    Missile(int startX);
    void moveUpward();
    int getX() const { return startX; };
    int getY() const { return startY; };
    int getRows() const { return rows; };
    int getCols() const { return cols; };
    friend bool checkForMissileCollisions(const Missile& m);
};

Missile::Missile(int startX) {
    this->startX = startX;
    displayFrame(startX, startY, missileRep);
}

bool checkForMissileCollisions(const Missile& m) {
    for (int i = 0; i < m.missileRep.size(); i++) {
        for (int j = 0; j < m.missileRep[i].size(); j++) {
            int screenX = m.startX + j;
            int screenY = m.startY + i;

            for (int obIndex = 0; obIndex < activeObstacles.size(); obIndex++) {
                Obstacle* ob = activeObstacles[obIndex];
                int obX = ob->getX();
                int obY = ob->getY();

                for (int oi = 0; oi < 2; oi++) {
                    for (int oj = 0; oj < 2; oj++) {
                        int obstacleX = obX + oj;
                        int obstacleY = obY + oi;

                        if (obstacleY == screenY + 1 && obstacleX == screenX) {
                            deleteObstacle(obIndex); // Delete the obstacle if it has collided with the missile
                            showCurrentScore(100); // Update the score by 100
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void Missile::moveUpward() {
    bool isColliding = checkForMissileCollisions(*this);

    if (!isColliding && startY != 1) {
        clearFrame(startX, startY, rows, cols); //DONE!
        startY--;
        displayFrame(startX, startY, missileRep);
    }
    Sleep(10);
}

vector<Missile*> activeMissiles; // Vector for counting active missiles

// Define the Aircraft class
class Aircraft {
private:
    vector <vector<int>> aircraftRep = {
        {0, 0, 11, 0, 0},
        {0, 10, 1, 110, 0},
        {10, 0, 1, 0, 110},
    };
    int startX = 54; // Starting X coordinate
    int startY = 25; // Starting Y coordinate
    const int rows = 3;
    const int cols = 5;
public:
    Aircraft();
    void shootMissile();
    void moveLeft();
    void moveRight();
    int getStartX() const { return startX; }
    int getStartY() const { return startY; }
    int getRows() const { return rows; }
    int getCols() const { return cols; }
    friend bool checkForAircraftCollisions(const Aircraft& a);
};

bool checkForAircraftCollisions(const Aircraft& a) {    
    for (int i = 0; i < a.aircraftRep.size(); i++) {
        for (int j = 0; j < a.aircraftRep[i].size(); j++) {
            if (a.aircraftRep[i][j] == 0) continue;

            // Real screen coordinates of current aircraft cell
            int screenY = a.startY + i;
            int screenX = a.startX + j;

            // Check if any obstacle is right above the current aircraft block
            for (Obstacle* ob : activeObstacles) {
                int obX = ob->getX();
                int obY = ob->getY();

                for (int oi = 0; oi < 2; oi++) { // obstacle height
                    for (int oj = 0; oj < 2; oj++) { // obstacle width

                        int obstacleBlockY = obY + oi;
                        int obstacleBlockX = obX + oj;
                        int margin = 1;
                        if (i == 0) margin = 0; // Top row: tight margin
                        else if (i == 1) margin = 1;
                        else if (i == 2) margin = 2; // Bottom row: wider margin

                        // Check if obstacle block is one row above the aircraft block or to the sides of the aircraft
                        // obstacleBlockX - screenX checks how close the obstacle is to the aicraft horizontally
                        if (obstacleBlockY == screenY - 1 &&
                            abs(obstacleBlockX - screenX) <= margin) {
                            return true;
                        }

                    }
                }
            }
        }
    }
    return false;
}

Aircraft::Aircraft() {
    displayFrame(startX, startY, aircraftRep);
}

// Make sure to take the obstacles in the view as well in the future
// Because there might be an obstacle to the left or the right and Aircraft could walk over it instead of stopping
void Aircraft::moveLeft() {
    int futureX = startX - 1;

    int originalX = startX;
    startX = futureX;

    bool isColliding = checkForAircraftCollisions(*this);

    startX = originalX;

    if (!isColliding && startX != 0) {
        clearFrame(startX, startY, rows, cols);
        startX--;
        displayFrame(startX, startY, aircraftRep);
    }
    else if (isColliding) {
        endGame();
    }
}

void Aircraft::moveRight() {
    int futureX = startX + 1;

    int originalX = startX;
    startX = futureX;

    bool isColliding = checkForAircraftCollisions(*this);

    startX = originalX;

    if (!isColliding && startX < getConsoleRightmostX() - 4) {
        clearFrame(startX, startY, rows, cols);
        startX++;
        displayFrame(startX, startY, aircraftRep);
    }
    else if (isColliding) {
        endGame();
    }
}

void Aircraft::shootMissile() {
    activeMissiles.push_back(new Missile(startX + 1));
}

void listenForEvents(Aircraft& a) {
    static bool spacePressed = false; // Added flag to prevent missile spam

    // Move left if the left arrow is pressed
    if (GetAsyncKeyState(VK_LEFT) & 0x8000) {
        a.moveLeft();
    }
    // Move right if the right arrow is pressed
    if (GetAsyncKeyState(VK_RIGHT) & 0x8000) {
        a.moveRight();
    }
    // Shoot the missile if space is pressed
    if (GetAsyncKeyState(VK_SPACE) & 0x8000) {
        if (!spacePressed) {  // Only fire when first pressed
            a.shootMissile();
            spacePressed = true;
        }
    }
    else {
        spacePressed = false; // Reset flag when key is released
    }
}

void makeObstacles() {
    if (activeObstacles.size() < 3) activeObstacles.push_back(new Obstacle); // Make sure that obstacles are limited in quantity
}

void deleteObstacle(int index) {
    //for (int i = 0; i < activeObstacles.size(); i++) {
        clearFrame(activeObstacles[index]->getX(), activeObstacles[index]->getY(), activeObstacles[index]->getRows(), activeObstacles[index]->getCols());
        delete activeObstacles[index];
        activeObstacles.erase(activeObstacles.begin() + index);
    //    i--; // Adjust index after deletion
    //}
}

void controlObstacles() {
    static int frameCount = 0; // Make the frame count static so it doesn't reset on every function call
    frameCount++; // Increase the frame count on every run

   if (frameCount % 5 == 0) {
        for (int i = 0; i < activeObstacles.size(); i++) {
            activeObstacles[i]->moveDown();  // Move the obstacle

            // Delete obstacle if it reaches the bottom
            if (activeObstacles[i]->getY() == activeObstacles[i]->getBottomLimit()) {
                deleteObstacle(i);
            }
        }
   }
}

void deleteMissile() {
    for (int i = 0; i < activeMissiles.size(); i++) {
    // Delete missile if it has reached the top or if it has collided with an obstacle
        clearFrame(activeMissiles[i]->getX(), activeMissiles[i]->getY(), activeMissiles[i]->getRows(), activeMissiles[i]->getCols());
        delete activeMissiles[i];
        activeMissiles.erase(activeMissiles.begin() + i);
        i--; // Adjust index after deletion
    }
}

void controlMissiles() {
    for (int i = 0; i < activeMissiles.size(); i++) {
        activeMissiles[i]->moveUpward();  // Move the missile

        // Delete missile if it has reached the top or if it has collided with an obstacle
        if (activeMissiles[i]->getY() == 1 || checkForMissileCollisions(*activeMissiles[i])) {
            deleteMissile();
        }
    }
}

void displayHighscore() {
    static int highscore = -1;  // -1 means "not loaded yet"

    // Load from file on first call only
    if (highscore == -1) {
        ifstream inFile("highscore.txt");
        inFile >> highscore;
        if (!inFile) {
            cout << "Error loading from file.";
            highscore = 0; // If file doesn't exist or is invalid
        }
        inFile.close();
    }

    setCursorPosition(50, 12);
    // Check and update
    if (score > highscore) {
        highscore = score;

        // Save new highscore to file
        ofstream outFile("highscore.txt");
        outFile << highscore;
        outFile.close();

        cout << "New highscore! " << highscore << endl;
    }
    else {
        cout << "Current highscore: " << highscore << endl;
    }
}

inline void endGame() {
    gameIsRunning = false;
    setCursorPosition(55, 10);
    cout << "Game Over!";
    setCursorPosition(48, 14);
    cout << "Press R to restart the game. \n";
}

inline void checkForGameOver(Aircraft& a) {
    // End the game if an obstacle has collided with the aircraft
    if (checkForAircraftCollisions(a)) {
        endGame();
        displayHighscore();
    }
}

void restartTheGame(Aircraft& a) {
    clearFrame(a.getStartX(), a.getStartY(), a.getRows(), a.getCols());
    gameIsRunning = true;
    setCursorPosition(55, 10);
    cout << "          ";
    setCursorPosition(50, 12);
    cout << "                       ";
    setCursorPosition(48, 14);
    cout << "                             ";
    setCursorPosition(0, 0);
    cout << "                   ";
    score = 0;
    gameLoop();
}

void gameLoop() {
    Aircraft a; // Create the aircaft
    showCurrentScore();

    // The outer loop is for keeping the console running in case the game is over
    while (true) {
        while (gameIsRunning) {
            listenForEvents(a); // Listen for user interactions with the aircraft
            makeObstacles(); // Keep creating obstacles while the game is running
            controlObstacles(); // Control the obstacles while the game is running
            controlMissiles(); // Control the missiles while the game is running
            checkForGameOver(a); // Keep checking if an obstacle hit the aircraft
            Sleep(10); // Control game speed
        }

        // If R is pressed after game over, reset everything
        if (GetAsyncKeyState('R') & 0x8000) {
            restartTheGame(a);
        }
    }
}

void cleanUpDynamicMemories() {
    for (Missile* m : activeMissiles) {
        delete m;
    }
    activeMissiles.clear();

    for (Obstacle* o : activeObstacles) {
        delete o;
    }
    activeObstacles.clear();
 }

int main()
{
    hideCursor(); // Hide the cursor
    gameLoop(); // Run the game loop to start the game
    cleanUpDynamicMemories(); // Cleanup remaining dynamically allocated memories
}
