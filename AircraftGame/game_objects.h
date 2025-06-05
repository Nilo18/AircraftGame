#pragma once
#ifndef GAME_OBJECTS_H
#define GAME_OBJECTS_H
#include <vector>
#include <random>
#include "game_motion.h"
#include "game_utils.h"
class BossMissile;
class Aircraft;
bool checkForBossMissileCollisions(const BossMissile& bm, const Aircraft& a);

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
    int getX() const noexcept;
    int getY() const noexcept;
    int getRows() const noexcept;
    int getCols() const noexcept;
    int getBottomLimit() const noexcept;
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
    int getX() const noexcept;
    int getY() const noexcept;
    int getRows() const noexcept;
    int getCols() const noexcept;
    friend bool checkForMissileCollisions(const Missile& m);
};

// Define the Aircraft class
class Aircraft {
private:
    vector <vector<int>> aircraftRep = {
        {0,   0,  101,  0,  0},
        {0,  101, 101, 101, 0},
        {101, 0,  101,  0, 101},
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
    int getStartX() const noexcept;
    int getStartY() const noexcept;
    int getRows() const noexcept;
    int getCols() const noexcept;
    friend bool checkForAircraftCollisions(const Aircraft& a);
    friend bool checkForBossMissileCollisions(const BossMissile& bm, const Aircraft& a);
};

class Boss {
private:
    vector <vector<int>> bossRep = {
        {1011, 0, 1011, 0, 1011},
        {0, 1011, 101, 1011, 0},
        {0,  0,   1011,  0,  0}
    };
    int startX = 55;
    int startY = 1;
    const int rows = 3;
    const int cols = 5;
    unsigned int hp = 25;
public:
    Boss();
    void shootMissile();
    void moveLeft();
    void moveRight();
    int getStartX() const noexcept;
    int getStartY() const noexcept;
    int getRows() const noexcept;
    int getCols() const noexcept;
    unsigned int getHp() const noexcept;
    void setHp(int newHp);
    friend bool checkForBossCollisions(const Boss& boss);
};

class BossMissile {
private:
    vector<vector<int>> bossMissileRep = {
        {0,   1011,   0},
        {1011, 101, 1011},
        {0,    1011,  0}
    };
    int startX;
    int startY;
    const int rows = 3;
    const int cols = 3;
    int bottomLimit = 25;
public:
    BossMissile(int startX, int startY = 5);
    void moveDown();
    void moveOnLeftDiagonal();
    void moveOnRightDiagonal();
    friend bool checkForBossMissileCollisions(const BossMissile& bm, const Aircraft& a);
    int getStartX() const noexcept;
    int getStartY() const noexcept;
    int getRows() const noexcept;
    int getCols() const noexcept;
    int getBottomLimit() const noexcept;
};

extern vector<Obstacle*> activeObstacles; // Vector for counting active obstacles, extern means it's defined elsewhere
extern vector<Missile*> activeMissiles; // Vector for counting active missiles
extern vector<BossMissile*> activeBossMissiles; // Vector for counting missiles shot by the boss

#endif