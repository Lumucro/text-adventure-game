#pragma once
#include "../Framework/stdafx.h"

class InputHandler;
class Inventory;
class Area;

class Game final
{

public:

	static Game* GetInstance();
	static void Destroy();

	//Start
	void Start(bool debugmode);
	//Kill player
	void KillPlayer();
	void Win();

	//Area setter
	bool SetCurrentArea(const string& areaRef);
	bool SetCurrentArea(Area* pArea);
	//Area getter
	Area* GetCurrentArea();
	Inventory* GetInventory();

private:

	//Singleton pattern
	Game();
	~Game();

	static Game* m_pInstance;
	
	//Load files
	bool InitializeContent();
	//Gameloop
	void GameLoop();

	//Keep game running
	bool m_GameRunning = true;
	bool m_PlayerIsAlive = true;
	bool m_DebugMode = false;

	//Needed other classes
	InputHandler* m_pInputHandler = nullptr;
	Inventory* m_pInventory = nullptr;
	//Current area
	Area* m_pCurrentArea = nullptr;
	//Starting area
	const string m_StartingArea = "cell_south";
};
