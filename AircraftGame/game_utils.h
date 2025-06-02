#pragma once
#ifndef GAME_UTILS_H
#define GAME_UTILS_H
#include <Windows.h>
#include <string>
#include <fstream>
#include "game_motion.h"
using namespace std;

extern int score;
extern int highscore;  // -1 means "not loaded yet"
void setCursorPosition(int x, int y);
HANDLE getConsole();
int getConsoleRightmostX();
int getConsoleBottomMostY();
void hideCursor();
void showCurrentScore(int add = 0);
void cleanUpDynamicMemories();
void clearMessage(int x, int y, const string& message);
void displayHighscore();
int generateRandomNumber(int start, int end);
void displayCurrentVersion();
void displayAllClear();
void disableConsoleResize();

#endif
