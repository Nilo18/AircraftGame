#include <vector>
#include <random>
#include "game_objects.h"
//#include "game_utils.h"  
//#include "game_motion.h"
using namespace std;
vector<Missile*> activeMissiles;
vector<Obstacle*> activeObstacles;

// Functions related to the Obstacle
int Obstacle::getX() const { return startX; }
int Obstacle::getY() const { return startY; }
int Obstacle::getRows() const { return rows; }
int Obstacle::getCols() const { return cols; }
int Obstacle::getBottomLimit() const { return bottomLimit; }

Obstacle::Obstacle() {
    this->startX = generateRandomNumber(2, getConsoleRightmostX() - 2);
    displayFrame(startX, startY, obstacleRep);
}

void Obstacle::moveDown() {
    clearFrame(startX, startY, rows, cols); // Pass the size of the obstacle as well as the coordinates DONE!
    if (startY < bottomLimit) startY++;
    displayFrame(startX, startY, obstacleRep);
}

// Functions related to the Missile
int Missile::getX() const { return startX; }
int Missile::getY() const { return startY; }
int Missile::getRows() const { return rows; }
int Missile::getCols() const { return cols; }

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
        clearFrame(startX, startY, rows, cols);
        startY--;
        displayFrame(startX, startY, missileRep);
    }
}

// Functions related to the aircraft
int Aircraft::getStartX() const { return startX; }
int Aircraft::getStartY() const { return startY; }
int Aircraft::getRows() const { return rows; }
int Aircraft::getCols() const { return cols; }

Aircraft::Aircraft() {
    displayFrame(startX, startY, aircraftRep);
}

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

// Check if the aircraft is colliding with an obstacle on the side while it is moving
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

// Functions related to the boss
int Boss::getStartX() const { return startX; }
int Boss::getStartY() const { return startY; }
int Boss::getRows() const { return rows; }
int Boss::getCols() const { return cols; }
int Boss::getHp() const { return hp; }

void Boss::moveLeft() {
    if (startX != 0) {
        clearFrame(startX, startY, rows, cols);
        startX--;
        displayFrame(startX, startY, bossRep);
    }
}

void Boss::moveRight() {
    if (startX < getConsoleRightmostX() - 4) {
        clearFrame(startX, startY, rows, cols);
        startX++;
        displayFrame(startX, startY, bossRep);
    }
}

Boss::Boss() {
    displayFrame(startX, startY, bossRep);
}
