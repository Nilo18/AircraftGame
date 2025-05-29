#include "game_motion.h" 
#include "game_utils.h" // Include this to access utility functions such as setCursorPosition
#include <iostream>
#include <Windows.h>
using namespace std;
bool gameIsRunning = true;

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
void displayFrame(int x, int y, vector<vector<int>> objectToDisplay) {
    for (int i = 0; i < objectToDisplay.size(); i++) {
        setCursorPosition(x, y + i);
        for (int j = 0; j < objectToDisplay[i].size(); j++) {
            int cell = objectToDisplay[i][j];

            if (cell == 0) {
                cout << ' ';
                continue;
            }
            else {
                switch (cell) {
                case 10: cout << '/'; break;
                case 110: cout << '\\'; break;
                case 111:
                    SetConsoleTextAttribute(getConsole(), FOREGROUND_GREEN | FOREGROUND_INTENSITY | BACKGROUND_GREEN | BACKGROUND_INTENSITY);
                    cout << '#'; break;
                case 101:
                    SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | BACKGROUND_RED);
                    cout << "o"; break;
                case 11:
                    SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY);
                    cout << '*'; break;
                default: cout << '|'; break;
                }
            }

            // Reset color back to white (default)
            SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
        }
    }
}

void deleteMissile(int index) {
    clearFrame(activeMissiles[index]->getX(), activeMissiles[index]->getY(), activeMissiles[index]->getRows(), activeMissiles[index]->getCols());
    delete activeMissiles[index];
    activeMissiles.erase(activeMissiles.begin() + index);
}

void controlMissiles() {
    for (int i = 0; i < activeMissiles.size(); ) {
        Missile* m = activeMissiles[i];

        if (m->getY() == 1 || checkForMissileCollisions(*m)) {
            deleteMissile(i); // Only delete the missile that triggered this condition
            continue; // Don't increment i
        }

        m->moveUpward();
        i++; // Only increment if not deleted
    }
}

void deleteObstacle(int index) {
    clearFrame(activeObstacles[index]->getX(), activeObstacles[index]->getY(), activeObstacles[index]->getRows(), activeObstacles[index]->getCols());
    delete activeObstacles[index];
    activeObstacles.erase(activeObstacles.begin() + index);
}

int obstacleCount = 1;
int lastScoreChecked = -10;

void makeObstacles() {
    // If the current score is a multiple of 1000, isn't 0 and obstacle count hasn't been increased at this multiple, then increase it
    if (score != 0 && score % 1000 == 0 && lastScoreChecked != score) {
        obstacleCount++;
        lastScoreChecked = score; // Mark the last score at which the obstacle count was increased
    }

    // If there are no active obstacles and the game is running, create the current amount of them (The quantity is regulated by the if statement above)
    if (activeObstacles.size() == 0 && gameIsRunning) {
        for (int i = 0; i < obstacleCount; i++) {
            activeObstacles.push_back(new Obstacle);
        }
    }
}

bool obstacleWasNaturallyDeleted = false;
void controlObstacles() {
    static int frameCount = 0; // Make the frame count static so it doesn't reset on every function call
    frameCount++; // Increase the frame count on every run, frames are counted to control only obstacle's speed

    if (frameCount % 15 == 0) {
        for (int i = 0; i < activeObstacles.size(); i++) {
            activeObstacles[i]->moveDown();  // Move the obstacle

            // Delete obstacle if it reaches the bottom
            if (activeObstacles[i]->getY() == activeObstacles[i]->getBottomLimit()) {
                deleteObstacle(i);
                obstacleWasNaturallyDeleted = true;
            }
            else {
                obstacleWasNaturallyDeleted = false;
            }
        }
    }
}

void endGame() {
    gameIsRunning = false;
    cleanUpDynamicMemories();
    setCursorPosition(55, 10);
    cout << "Game Over!";
    setCursorPosition(48, 14);
    cout << "Press R to restart the game.\n";
}
