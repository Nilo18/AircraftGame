#pragma once
#ifndef GAME_MOTION_H
#define GAME_MOTION_H
#include <vector>
#include "game_objects.h" // Include this to access activeMissiles vector
using namespace std;

extern bool gameIsRunning;
extern int obstacleCount; // Current obstacle count
extern int lastScoreChecked; // A flag to mark the last score at which the obstacle count was increased, it is initialized as -10 to ensure first test passes
void clearFrame(int x, int y, int rows, int cols);
void displayFrame(int x, int y, vector<vector<int>> objectToDisplay);
void deleteMissile(int index);
void controlMissiles();
void deleteObstacle(int index);
void makeObstacles();
void controlObstacles();
void endGame();

extern bool obstacleWasNaturallyDeleted; // A flag to check if the obstacle was deleted naturally (reached the bottom) or was shot down
#endif
