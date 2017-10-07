#pragma once

//Includes
#include <iostream>
#include <string>
#include <vector>
#include <map>

#include "../Framework/ContentManager.h"
#include "../Framework/Outputter.h"
#include "../Game/Game.h"

//Defines
#define OUTPUT (Outputter::GetInstance())
#define CONTENT (ContentManager::GetInstance())
#define GAME (Game::GetInstance())

//Namespace
using namespace std;

//Structs and whatnot
enum class AREADIRECTION
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3,
	NONE = 4
};

enum class INTERACTABLE_USETYPE
{
	NOTHING = 0,
	DEATH = 1,
	UNLOCK = 2,
	WIN = 3
};
