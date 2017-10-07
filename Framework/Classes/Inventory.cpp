#include "Inventory.h"
#include "Interactable.h"
#include "../../Framework/Classes/Area.h"

Inventory::Inventory(bool debugmode) : m_DebugMode(debugmode)
{
}

Inventory::~Inventory()
{
}

void Inventory::PickupItem(Interactable* interactable, bool force)
{
	if (m_DebugMode) OUTPUT->Log("Picking up item " + interactable->GetName() + "\n", OUTPUT_COLOR::RED);

	//Make sure the item is valid
	if (!interactable && m_DebugMode)
	{
		OUTPUT->Log("\nPicking up item failed, invalid pointer!\nThis is bad, something broke. Panic and shout incomprehensibly, please.\n\n", OUTPUT_COLOR::RED);
		return;
	}

	//Remove it from the area
	if (GAME->GetCurrentArea()->RemoveInteractable(interactable->GetID()) || force)
	{
		//Add item to our inventory
		m_InteractablesArr[interactable->GetID()] = interactable;
		//Output
		OUTPUT->Log("You pick up " + interactable->GetName() + ".\n\n");
	}
}

void Inventory::DropItem(Interactable* interactable)
{
	if (m_DebugMode) OUTPUT->Log("Dropping item " + interactable->GetName() + "\n", OUTPUT_COLOR::RED);

	//Make sure the item is valid
	if (!interactable && m_DebugMode)
	{
		OUTPUT->Log("Dropping item failed, invalid pointer!\n", OUTPUT_COLOR::RED);
		return;
	}

	auto it = m_InteractablesArr.find(interactable->GetID());
	if (it != m_InteractablesArr.end())
	{
		//Add the item to the current area
		GAME->GetCurrentArea()->AddInteractable(interactable);
		//Remove it from our inventory
		m_InteractablesArr.erase(it);
		//Output
		OUTPUT->Log("You drop " + interactable->GetName() + ".\n\n");
	}
}

void Inventory::DropItem(const string& interactableName)
{
	//Check if the name happens to be the id
	map<string, Interactable*>::iterator it = m_InteractablesArr.find(interactableName);
	if (it != m_InteractablesArr.end())
	{
		DropItem(it->second);
		return;
	}

	//Check if any of the items has this name as an identifier
	for (auto interactable : m_InteractablesArr)
	{
		auto identifiers = interactable.second->GetIdentifiers();
		for (auto id : identifiers)
		{
			if (id == interactableName)
			{
				DropItem(interactable.second);
				return;
			}
		}
	}
}

bool Inventory::HasSpace()
{
	return (m_InteractablesArr.size() < m_MaxCarry);
}

void Inventory::LookAtInventory()
{
	if (m_InteractablesArr.size() == 0)
	{
		OUTPUT->Log("You aren't holding any items.\n\n");
	}
	else
	{
		//Just print the inventory contents
		OUTPUT->Log("You are holding the following items:\n");

		string name = "";
		for (auto it : m_InteractablesArr)
		{
			name = it.second->GetName();
			//Capitalize first letter
			name[0] = toupper(name[0]);
			OUTPUT->Log("-" + name + "\n");
		}
		OUTPUT->Log("\n");
	}
}
