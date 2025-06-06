#include <vector>
#include <random>
#include <iostream>
#include <string>
#include "game_objects.h"
//#include "game_utils.h"  
#include "game_motion.h"
using namespace std;
vector<Missile*> activeMissiles;
vector<Obstacle*> activeObstacles;
vector<BossMissile*> activeBossMissiles;

// Functions related to the Obstacle
int Obstacle::getX() const noexcept { return startX; }
int Obstacle::getY() const noexcept { return startY; }
int Obstacle::getRows() const noexcept { return rows; }
int Obstacle::getCols() const noexcept { return cols; }
int Obstacle::getBottomLimit() const noexcept { return bottomLimit; }

Obstacle::Obstacle() {
    this->startX = generateRandomNumber(2, getConsoleRightmostX() - 2);
    displayFrame(startX, startY, obstacleRep);
}

void Obstacle::moveDown() {
    clearFrame(startX, startY, rows, cols); // Pass the size of the obstacle as well as the coordinates 
    if (startY < bottomLimit) startY++;
    displayFrame(startX, startY, obstacleRep);
}

// Functions related to the Missile
int Missile::getX() const noexcept { return startX; }
int Missile::getY() const noexcept { return startY; }
int Missile::getRows() const noexcept { return rows; }
int Missile::getCols() const noexcept { return cols; }

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
int Aircraft::getStartX() const noexcept { return startX; }
int Aircraft::getStartY() const noexcept { return startY; }
int Aircraft::getRows() const noexcept { return rows; }
int Aircraft::getCols() const noexcept { return cols; }

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
int Boss::getStartX() const noexcept { return startX; }
int Boss::getStartY() const noexcept { return startY; }
int Boss::getRows() const noexcept { return rows; }
int Boss::getCols() const noexcept { return cols; }
int Boss::getHp() const noexcept { return hp; }
void Boss::setHp(int newHp) { this->hp += newHp; }

void Boss::takeDamage(int damage) {
    hp -= damage;
    if (hp < 0) hp = 0;

    string msg = string("Boss HP: ") + (hp < 10 ? " " : "") + to_string(hp) + "/" + to_string(25 * bossSpawnCounter);
    // Always ensure it's the same length (or pad it)
    msg += "  "; // extra padding if necessary
    setCursorPosition(15, 0);
    cout << msg;
}

void Boss::moveLeft() {
    if (startX != 0) {
        clearFrame(startX, startY, rows, cols);
        startX--;
        displayFrame(startX, startY, bossRep);
    }
}

void Boss::moveRight() {
    if (startX != getConsoleRightmostX() - 4) {
        clearFrame(startX, startY, rows, cols);
        startX++;
        displayFrame(startX, startY, bossRep);
    }
}

Boss::Boss() {
    displayFrame(startX, startY, bossRep);
}

void Boss::shootMissile() {
    //for (int i = 0; i < 2; i++) {
    //    activeBossMissiles.push_back(new BossMissile);
    //}
    //setCursorPosition(20, 0);
    //cout << "Shooting a missile";
    activeBossMissiles.push_back(new BossMissile(startX + 1));
}

bool checkForBossCollisions(const Boss& boss) {
    int bossStartX = boss.startX;
    int bossStartY = boss.startY;
    for (int bi = 0; bi < boss.bossRep.size(); bi++) {
        for (int bj = 0; bj < boss.bossRep[bi].size(); bj++) {
            if (boss.bossRep[bi][bj] == 0) continue;

            int bossPositionX = bossStartX + bj;
            int bossPositionY = bossStartY + bi;

            for (int i = 0; i < activeMissiles.size(); i++) {
                int missileX = activeMissiles[i]->getX();
                int missileY = activeMissiles[i]->getY();

                for (int mi = 0; mi < activeMissiles[i]->getRows(); mi++) {
                    for (int mj = 0; mj < activeMissiles[i]->getCols(); mj++) {
                        int missilePositionX = missileX + mi;
                        int missilePositionY = missileY + mj;
                        int margin = 1;
                        if (i == 0) margin = 0; // Top row: tight margin
                        else if (i == 1) margin = 1;
                        else if (i == 2) margin = 2; // Bottom row: wider margin

                        if (abs(missilePositionX - bossPositionX) <= margin && missilePositionY == bossPositionY + 1) {
                            deleteMissile(i); // Delete the player missile if it is colliding with the boss
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

// Functions related to boss' missiles

int BossMissile::getStartX() const noexcept { return startX; }
int BossMissile::getStartY() const noexcept { return startY; }
int BossMissile::getRows() const noexcept { return rows; }
int BossMissile::getCols() const noexcept { return cols; }
int BossMissile::getBottomLimit() const noexcept { return bottomLimit; }

// Refer back here if there are any collision problems
bool checkForBossMissileCollisions(const BossMissile& bm, const Aircraft& a) {
    int missileX = bm.startX;
    int missileY = bm.startY;
    int aircraftX = a.getStartX();
    int aircraftY = a.getStartY();
    for (int ai = 0; ai < a.aircraftRep.size(); ai++) {
        for (int aj = 0; aj < a.aircraftRep[ai].size(); aj++) {
            if (a.aircraftRep[ai][aj] == 0) continue;

            int aircraftGlobalX = aircraftX + aj;
            int aircraftGlobalY = aircraftY + ai;

            for (int mi = 0; mi < bm.rows; mi++) {
                for (int mj = 0; mj < bm.cols; mj++) {
                    if (bm.bossMissileRep[mi][mj] == 0 && !(mi == 2 && (mj == 0 || mj == 2))) continue;

                    int globalMissileX = missileX + mj;
                    int globalMissileY = missileY + mi;

                    if (globalMissileX == aircraftGlobalX && globalMissileY == aircraftGlobalY - 1) {
                        //setCursorPosition(0, 0);
                        //cout << "Collision at (" << globalMissileX << ", " << globalMissileY << ")\n";
                        return true;
                    }
                }
            }
            
        }
    }
    return false;
}

BossMissile::BossMissile(int startX, int startY) {
    this->startX = startX;
    this->startY = startY;
    displayFrame(startX, startY, bossMissileRep);

}

void BossMissile::moveDown() {
    clearFrame(startX, startY, rows, cols);
    if (startY < bottomLimit) startY++;
    displayFrame(startX, startY, bossMissileRep);
}