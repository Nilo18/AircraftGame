#pragma once
#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H
#include <vector>
#include <random>
#include "game_motion.h"
#include "game_utils.h"

using namespace std;

// Define an obstacle 
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
    int getX() const;
    int getY() const;
    int getRows() const;
    int getCols() const;
    int getBottomLimit() const;
};

// Define a missile
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
    int getX() const;
    int getY() const;
    int getRows() const;
    int getCols() const;
    friend bool checkForMissileCollisions(const Missile& m);
};

// Define the Aircraft class
class Aircraft {
private:
    vector <vector<int>> aircraftRep = {
        {0, 0, 101, 0, 0},
        {0, 101, 101, 101, 0},
        {101, 0, 101, 0, 101},
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
    int getStartX() const;
    int getStartY() const;
    int getRows() const;
    int getCols() const;
    friend bool checkForAircraftCollisions(const Aircraft& a);
};

extern vector<Obstacle*> activeObstacles; // Vector for counting active obstacles, extern means it's defined elsewhere
extern vector<Missile*> activeMissiles; // Vector for counting active missiles


#endif