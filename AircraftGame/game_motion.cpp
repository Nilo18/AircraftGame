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
                case 1011:
                    SetConsoleTextAttribute(getConsole(), FOREGROUND_RED | FOREGROUND_GREEN | BACKGROUND_RED | BACKGROUND_GREEN);
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
    if (activeObstacles.size() == 0 && gameIsRunning && !bossWasSpawned) {
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

Boss* boss = nullptr; 
int bossHp;
bool bossWasSpawned = false; // Flag to make sure that the boss is spawned once
int bossFrameCount = 0; // This will count the frames in which the boss moves/operates
int direction; // This will be the direction in which the boss will move
void spawnBoss() {
    // If the score is a multiple of 5000, game is running and the boss hasn't spawned, reassign the dynamically allocated memory for the boss from nullptr to Boss object
    if (score % 5000 == 0 && gameIsRunning && !bossWasSpawned) {
        boss = new Boss;
        bossHp = boss->getHp();
        bossWasSpawned = true; // Mark the flag as true since the boss has already spawned
        direction = generateRandomNumber(1, 2); // Generate the initial direction randomly, either left (1) or right (2)
    }
}

int bossStepCounter = 0; // This will count boss' steps
int maxSteps = 15; // This is the max amount of steps (distance) boss can walk
bool isPaused = false; // Flag to control the pause in between boss movements
int pauseCounter = 0;  // This will count through the duration of the pause 
int pauseDuration = 20; // This is the duration of the pause in between movement
bool bossShotAMissile = false;
void controlBoss() {
    // Check if the boss has spawned and the memory for it is allocated properly
    //int playerX = a.getStartX();
    if (bossWasSpawned && boss != nullptr) {
        bossFrameCount++; // If the boss has spawned, increase the frame count, this will now count the frames it covers

        // Move the boss every 4 frames, make sure that it's steps are in the boundaries
        if (bossFrameCount % 2 == 0 && bossStepCounter < maxSteps && direction == 1) {
            boss->moveLeft(); // If the direction is 1, boss shall move to the left
            bossStepCounter++; // Increase step count as it moves
        }
        else if (bossFrameCount % 2 == 0 && bossStepCounter < maxSteps && direction == 2) {
            boss->moveRight(); // If the direction is 2, boss shall move to the right
            bossStepCounter++; // Increase step count as it moves
        }
        if (bossStepCounter >= maxSteps) {
            isPaused = true; // If the boss has taken max allowed steps, stop it before changing direction
        }

        // Check if the boss has been paused (stopped moving)
        if (isPaused) {
            pauseCounter++; // If it has start counting through the duration of it's pause
            if (!bossShotAMissile) {
                boss->shootMissile();
                bossShotAMissile = true;
            }
            controlBossMissiles();
            // Check if the counter has exceeded the limit
            if (pauseCounter >= pauseDuration) {
                direction = generateRandomNumber(1, 2); // If it has, generate a new direction randomly 
                bossStepCounter = 0; // Reset it's step counter
                isPaused = false; // Reset the flag so the boss can move again
                bossShotAMissile = false;
                pauseCounter = 0; // Reset the pause counter
            }
        }

        if (checkForBossCollisions(*boss)) {
            bossHp--;
            boss->setHp(bossHp);
        }
    }
}

void deleteBossMissile(int index) {
    clearFrame(activeBossMissiles[index]->getStartX(), activeBossMissiles[index]->getStartY(), activeBossMissiles[index]->getRows(), activeBossMissiles[index]->getCols());
    delete activeBossMissiles[index];
    activeBossMissiles.erase(activeBossMissiles.begin() + index);
}

void controlBossMissiles() {
    //setCursorPosition(20, 0);
    //cout << "Controlling boss missiles...";
    for (int i = 0; i < activeBossMissiles.size(); i++) {
        activeBossMissiles[i]->moveDown();

        if (activeBossMissiles[i]->getStartY() == activeBossMissiles[i]->getBottomLimit()) {
            deleteBossMissile(i);
        }
    }
}
  

