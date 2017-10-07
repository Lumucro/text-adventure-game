#pragma once
#include "../stdafx.h"

class Inventory final
{

public:

	Inventory(bool debugmode);
	~Inventory();

	void PickupItem(Interactable* interactable, bool force = false);
	void DropItem(Interactable* interactable);
	void DropItem(const string& identifier);
		
	bool HasSpace();
	void LookAtInventory();

	const map<string, Interactable*> GetInteractables() { return m_InteractablesArr; }

private:


	//Debugmode
	bool m_DebugMode = false;
	//Max interactables we can carry
	const size_t m_MaxCarry = 2;

	//Map of items, ContentManager cleans these up so don't delete
	map<string, Interactable*> m_InteractablesArr;
};
