#include "game_utils.h"
#include "game_motion.h"
#include <iostream>
using namespace std;
//#include <Windows.h>
int score = 0;

// Function for setting cursor position inside the console
void setCursorPosition(int x, int y) {
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(getConsole(), coord);
}

// Get handle to the console
HANDLE getConsole() {
    return GetStdHandle(STD_OUTPUT_HANDLE);
}

// Function for getting the right most edge of the console
int getConsoleRightmostX() {
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

    // Add some safety
    if (GetConsoleScreenBufferInfo(getConsole(), &screenBufferInfo)) {
        return screenBufferInfo.srWindow.Right;
    }

    return -1; // Return -1 if there's an error
}

int getConsoleBottomMostY() {
    CONSOLE_SCREEN_BUFFER_INFO screenBufferInfo;

    if (GetConsoleScreenBufferInfo(getConsole(), &screenBufferInfo)) {
        return screenBufferInfo.srWindow.Bottom;
    }

    return -1; // Return -1 if there's an error
}

// Function for hiding the cursor inside the console
void hideCursor() {
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE); // Get handle to the console
    CONSOLE_CURSOR_INFO cursorInfo; // Initialize the class containing cursor info
    GetConsoleCursorInfo(consoleHandle, &cursorInfo);
    cursorInfo.bVisible = false;  // Make the cursor disappear
    SetConsoleCursorInfo(consoleHandle, &cursorInfo);
}

// Function for showing the current score
void showCurrentScore(int add) {
    if (add != 0) score += add;
    setCursorPosition(55, 0);
    cout << "Score: " << score << endl;
}

// Function for cleaning up remaining dynamically allocated memories
void cleanUpDynamicMemories() {
    for (Missile* m : activeMissiles) {
        clearFrame(m->getX(), m->getY(), m->getRows(), m->getCols());
        delete m;
    }
    activeMissiles.clear();

    for (Obstacle* o : activeObstacles) {
        clearFrame(o->getX(), o->getY(), o->getRows(), o->getCols());
        delete o;
    }
    activeObstacles.clear();
}

// Function for dynamically clearing game over messages
void clearMessage(int x, int y, const string& message) {
    setCursorPosition(x, y);
    cout << string(message.length(), ' '); // Print the string of empty spaces which will match the length of the given message 
}

int highscore = -1;
void displayHighscore() {
    // Load from file on first call only
    if (highscore == -1) {
        ifstream inFile("highscore.txt");
        inFile >> highscore;
        if (!inFile) {
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

// Function for generating random numbers
int generateRandomNumber(int start, int end) {
    random_device rd;
    default_random_engine dre(rd());
    uniform_int_distribution <int> dis(start, end);

    return dis(dre);
}

void displayCurrentVersion() {
    setCursorPosition(getConsoleRightmostX() - 17, 0);
    cout << "AircraftGame v1.3";
}

void displayAllClear() {
    // Start displaying "All Clear!" only if there is more than one active obstacle
    if (obstacleCount > 1) {
        static int frameCount = 0;
        frameCount++;
        // If the game is running the boss has not spawned, there are no obstacles and the obstacles weren't naturally deleted (i.e were all shot down) print a message "All Clear!"
        if (gameIsRunning && activeObstacles.empty() && !obstacleWasNaturallyDeleted && !bossWasSpawned) {
            setCursorPosition(0, 0);
            SetConsoleTextAttribute(getConsole(), 14);
            cout << "All clear!";
        }
        // Delete the message after 250 frames
        if (frameCount % 200 == 0) {
            setCursorPosition(0, 0);
            cout << "          ";
        }
    }
}

void disableConsoleResize() {
    HWND console = GetConsoleWindow();
    LONG style = GetWindowLong(console, GWL_STYLE);
    style &= ~WS_SIZEBOX;  // Disable resizing border
    SetWindowLong(console, GWL_STYLE, style);
}
