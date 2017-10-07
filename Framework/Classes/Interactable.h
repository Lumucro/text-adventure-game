#pragma once
#include "../stdafx.h"

using namespace std;

class Interactable final
{

public:

	Interactable(const string& ID);
	virtual ~Interactable();

	const bool Use(Interactable* otherItem);

	//Setters
	void AddIdentifier(const string& identifier);
	void SetName(const string& name);
	void AddDescription(const string& desc);
	void SetAreaExamineText(const string& text);
	void SetCanPickup(bool canPickup);
	void SetUseItem(const string& itemIdentifier);
	void SetPickupItem(const string& itemIdentifier);
	void SetPickupText(const string& text) { m_PickupText = text; }
	void SetAlreadyPickedUptext(const string& text) { m_PickupAlreadyGivenText = text; }
	void SetUnlockDirection(AREADIRECTION dir) { m_UnlockDirection = dir; }
	void SetUseType(INTERACTABLE_USETYPE type) { m_UseType = type; }
	void SetUseText(const string& text) { m_UseText = text; }
	void SetUseFailText(const string& text) { m_UseFailText = text; }
	//Getters
	const string GetID() { return m_ID; }
	const vector<string> GetIdentifiers() { return m_Identifiers;  }
	const string GetName() { return m_Name; }
	const string GetRandomDescription();
	const string GetAreaExamineText() { return m_AreaExamineText; }
	bool CanPickup() { return m_CanPickup; }
	const bool HasPickupItem() { return m_PickupItem != ""; }
	const string GetPickupItem();
	const string GetPickupText() { return m_PickupText; }
	const AREADIRECTION GetUnlockDirection() { return m_UnlockDirection; }
	const INTERACTABLE_USETYPE GetUseType() { return m_UseType; }
	const string GetUseText() { return m_UseText; }
	const string GetUseFailText() { return m_UseFailText; }

protected:

	//ID
	string m_ID = "";
	//Identifiers
	vector<string> m_Identifiers;
	//Name
	string m_Name = "";

	//What item does this give
	string m_PickupItem = "";
	bool m_AlreadyGaveItem = false;
	string m_PickupAlreadyGivenText = "";
	string m_PickupText = "";

	//What item does this get used on + what does this do
	INTERACTABLE_USETYPE m_UseType = INTERACTABLE_USETYPE::NOTHING;
	string m_UseItem = "";
	string m_UseText = "";
	string m_UseFailText = "";
	//For keys, what direction does this unlock when used
	AREADIRECTION m_UnlockDirection = AREADIRECTION::NONE;

	//Can this item be picked up
	bool m_CanPickup = false;
	//Descriptions and area examine text
	vector<string> m_Descriptions;
	string m_AreaExamineText = "";

};
