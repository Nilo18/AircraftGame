#include <iostream>
#include "game_utils.h"
#include "game_motion.h"
#include "game_objects.h"
using namespace std;
void gameLoop();

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

void checkForGameOver(Aircraft& a) {
    // End the game if an obstacle has collided with the aircraft
    if (checkForAircraftCollisions(a)) {
        endGame();
        displayHighscore();
    }
}

void restartTheGame(Aircraft& a) {
    clearFrame(a.getStartX(), a.getStartY(), a.getRows(), a.getCols());
    gameIsRunning = true;
    clearMessage(55, 10, "Game Over!");
    // Since highscore is dynamic, count it's length based on the size of the number
    string countedHighscore; 
    if (score > highscore) countedHighscore = "New highscore! " + to_string(highscore);
    else { countedHighscore = "Current highscore: " + to_string(highscore); }
    clearMessage(50, 12, countedHighscore); // Now pass the counted string to the clearing function
    clearMessage(48, 14, "Press R to restart the game.");
    // Do the same for score
    string countedScore = "Score: " + to_string(score);
    clearMessage(55, 0, countedScore);
    score = 0;
    obstacleCount = 1;
    gameLoop(); // Rerun the gameloop to create everything all over
}

void gameLoop() {
    Aircraft a; // Create the aircaft
    showCurrentScore(); // Show the current score
    displayCurrentVersion(); // Show the current version of the game

    // The outer loop is for keeping the console running in case the game is over
    while (true) {
        while (gameIsRunning) {
            listenForEvents(a); // Listen for user interactions with the aircraft
            makeObstacles(); // Keep creating obstacles while the game is running
            controlObstacles(); // Control the obstacles while the game is running
            controlMissiles(); // Control the missiles while the game is running
            checkForGameOver(a); // Keep checking if an obstacle hit the aircraft
            displayAllClear(); // Display all clear if the player shoots all the obstacles down
            spawnBoss(); // Spawn boss if the conditions are met (The function check for the conditions)
            controlBoss(); // Control the boss
            Sleep(10); // Control game speed
        }

        // If R is pressed after game over, reset everything
        if (GetAsyncKeyState('R') & 0x8000) {
            restartTheGame(a);
        }
    }
}

int main()
{
    hideCursor(); // Hide the cursor
    gameLoop(); // Run the game loop to start the game
}