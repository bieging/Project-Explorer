#ifndef MAIN_H
#define MAIN_H

#pragma once

//#include "BMath.h"

enum PLAYER_GRAVITY_STATE
{
	GRAVITY,
	FIXED_H_FLY,
	FREE_FLY
};

enum GAME_STATE
{
	WELCOME,
	START,
	MENU,
	ACTIVE,
	INFORMATION
};

PLAYER_GRAVITY_STATE pgs = GRAVITY;
GAME_STATE gs = WELCOME;
GAME_STATE lastGameState = WELCOME;

#define INPUT_PRESSED 0
#define INPUT_HOLD 1
#define INPUT_RELEASED 2

class Main
{
public:
	Main();
	~Main();
};

#endif // !MAIN_H
