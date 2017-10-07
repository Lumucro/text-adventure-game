#include "Game.h"
#include "../Framework/InputHandler.h"
#include "../Framework/Classes/Inventory.h"

Game* Game::m_pInstance = nullptr;

Game* Game::GetInstance()
{
	if (!m_pInstance)
		m_pInstance = new Game();

	return m_pInstance;
}

void Game::Destroy()
{
	if (m_pInstance)
		delete m_pInstance;
}

Game::Game()
{
}

Game::~Game()
{
	//Clean up inputhandler and inventory
	delete m_pInputHandler;
	delete m_pInventory;
	//Cleanup logger and contentmanager singletons
	OUTPUT->Destroy();
	CONTENT->Destroy();
}

void Game::Start(bool debugmode)
{
	//Debugmode
	m_DebugMode = debugmode;
	//Inputhandler
	m_pInputHandler = new InputHandler(m_DebugMode);
	//Inventory
	m_pInventory = new Inventory(m_DebugMode);

	//Load in files like items, areas etc
	if (InitializeContent())
	{
		//Set the starting area
		if (m_DebugMode) OUTPUT->Log("Setting starting area to " + m_StartingArea +"\n", OUTPUT_COLOR::GREEN);
		m_pCurrentArea = CONTENT->GetArea(m_StartingArea);

		//Starting text
		OUTPUT->Log("<type 'help' for a list of commands>\n\n\n", OUTPUT_COLOR::YELLOW);
		OUTPUT->Log("You wake up in an unknown location.\nSomeone must have knocked you out and taken all your stuff.\n\n");

		//If that worked, start the gameloop
		if (m_DebugMode) OUTPUT->Log("Starting gameplay loop...\n\n", OUTPUT_COLOR::GREEN);
		while (m_GameRunning && m_PlayerIsAlive) GameLoop();

		//Pause after death, the player needs to still be able to see it
		cin.get();
	}
	else
	{
		OUTPUT->Log("Something went wrong trying to initialize game content!\n", OUTPUT_COLOR::RED);
		cin.get();
	}
}

void Game::KillPlayer()
{
	m_PlayerIsAlive = false;
	OUTPUT->Log("\nAs you fall to the ground, your soul leaves your body and prepares for eternal damnation.\nYou are dead.\n\n", OUTPUT_COLOR::RED);
}

void Game::Win()
{
	m_PlayerIsAlive = false;
	OUTPUT->Log("\nFreedom is yours for the taking, as you escape the dark dungeon.\nYou have survived the Goblin prison!\n\n", OUTPUT_COLOR::GREEN);
}

bool Game::SetCurrentArea(const string& areaRef)
{
	Area* area = CONTENT->GetArea(areaRef);
	if (area == nullptr)
	{
		return false;
	}

	m_pCurrentArea = CONTENT->GetArea(areaRef);
	return true;
}

bool Game::SetCurrentArea(Area* pArea)
{
	//In case the pointer is invalid, report a failure
	if (pArea == nullptr)
	{
		OUTPUT->Log("Failed to set area (invalid pointer)!\n", OUTPUT_COLOR::RED);
		return false;
	}

	m_pCurrentArea = pArea;
	return true;
}

Area* Game::GetCurrentArea()
{
	return m_pCurrentArea;
}

Inventory* Game::GetInventory()
{
	return m_pInventory;
}

bool Game::InitializeContent()
{
	//Load content
	return CONTENT->Load(m_DebugMode);
}

void Game::GameLoop()
{
	//Handle input
	m_pInputHandler->Tick();
}