#pragma once
#include "Interactable.h"
#include <time.h>
#include "Area.h"

Interactable::Interactable(const string& idRef) : m_ID(idRef)
{
}

Interactable::~Interactable()
{
}

const bool Interactable::Use(Interactable* otherItem)
{
	if (!otherItem && m_ID == m_UseItem)
	{
		OUTPUT->Log(m_UseText + "\n\n");

		switch (m_UseType)
		{
		case INTERACTABLE_USETYPE::DEATH:
			GAME->KillPlayer();
			break;
		case INTERACTABLE_USETYPE::UNLOCK:
			GAME->GetCurrentArea()->SetLocked(m_UnlockDirection, false);
			GAME->GetCurrentArea()->RemoveInteractable(m_ID);
			break;
		case INTERACTABLE_USETYPE::WIN:
			GAME->Win();
			break;
		}

		return true;
	}
	else if (otherItem)
	{
		if (otherItem->GetID() == m_UseItem)
		{
			OUTPUT->Log(otherItem->GetUseText() + "\n\n");

			switch (otherItem->GetUseType())
			{
			case INTERACTABLE_USETYPE::DEATH:
				GAME->KillPlayer();
				break;
			case INTERACTABLE_USETYPE::UNLOCK:
				GAME->GetCurrentArea()->SetLocked(otherItem->GetUnlockDirection(), false);
				GAME->GetCurrentArea()->RemoveInteractable(otherItem->GetID());
				break;
			case INTERACTABLE_USETYPE::WIN:
				GAME->Win();
				break;
			}

			return true;
		}

	}

	string failtext = "";
	if (otherItem) if (otherItem->GetUseFailText() != "") failtext = otherItem->GetUseFailText();
	if (m_UseFailText != "") failtext = m_UseFailText;
	
	if (failtext == "") OUTPUT->Log("That doesn't seem to do anything.\n\n");
	else OUTPUT->Log(failtext + "\n\n");

	return false;
}

void Interactable::AddIdentifier(const string& identifier)
{
	if (identifier != "")
		m_Identifiers.push_back(identifier);
}

void Interactable::SetName(const string& name)
{
	m_Name = name;
}

void Interactable::AddDescription(const string& desc)
{
	m_Descriptions.push_back(desc);
}

void Interactable::SetAreaExamineText(const string& text)
{
	m_AreaExamineText = text;
}

void Interactable::SetCanPickup(bool canPickup)
{
	m_CanPickup = canPickup;
}

void Interactable::SetUseItem(const string& itemIdentifier)
{
	m_UseItem = itemIdentifier;
}

void Interactable::SetPickupItem(const string& itemIdentifier)
{
	m_PickupItem = itemIdentifier;
}

const string Interactable::GetRandomDescription()
{
	string out = "You can't make out what this is.";

	//Are there enough descriptions to get a random one?
	int size = m_Descriptions.size();
	if (size > 0)
	{
		//More than one, get a random one
		//Reset rand
		srand(size_t(time(NULL)));
		//Get a random
		int randomIndex = rand() % size;
		out = m_Descriptions[randomIndex];
	}

	return out;
}

const string Interactable::GetPickupItem()
{
	//If we didn't already give the item
	if (!m_AlreadyGaveItem)
	{
		m_AlreadyGaveItem = true;
		return m_PickupItem;
	}

	//Else return nothing
	OUTPUT->Log(m_PickupAlreadyGivenText + "\n\n");
	return "";
}
