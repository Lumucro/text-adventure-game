#include "Area.h"
#include "Interactable.h"

Area::Area(const string& identifierRef) : m_Identifier(identifierRef)
{
}

Area::~Area()
{
}

void Area::Enter()
{
	//For one-time text popups, eg: as you enter the room, you activate a trap 
	//(we dont actually need to do any events because we can set it up to already be like that, just need to pretend we did for the player)
	//This will be, by design, always a 'bad' or 'oh shit' thing, so red text suits it
	if (m_FirstEntry)
	{
		OUTPUT->Log(m_FirstEntryText + "\n", OUTPUT_COLOR::PINK);
		m_FirstEntry = false;
	}

	OUTPUT->Log("You find yourself in " + m_Name + ".\n\n");
}

void Area::SetName(const string& name)
{
	m_Name = name;
}

void Area::SetExamine(const string& text)
{
	m_ExamineText = text;
}

void Area::AddAmbient(const string& ambient)
{
	m_AmbientVec.push_back(ambient);
}

void Area::SetLocked(AREADIRECTION dir, const bool locked)
{
	m_LockedAreasArr[int(dir)] = locked;
}

Area* Area::GetConnectedArea(AREADIRECTION area)
{
	//Get the connected area pointer for the given direction
	return m_ConnectedAreasPointersArr[area];
}

void Area::AddInteractable(Interactable* objRef)
{
	//Return index of new interactable
	m_InteractablesArr.insert(pair<string, Interactable*>(objRef->GetID(), objRef));
}

bool Area::RemoveInteractable(const string& identifierRef)
{
	//Find the interactable
	auto it = m_InteractablesArr.find(identifierRef);

	//Erase it from the map
	if (it != m_InteractablesArr.end())
	{
		m_InteractablesArr.erase(it);
		return true;
	}

	return false;
}

void Area::SetConnectedArea(AREADIRECTION dir, const TemporaryConnection& tempArea)
{
	//Remember what area should be in what direction for PostLoad
	m_ConnectedAreasArr[dir] = tempArea;
}

const string Area::Examine()
{
	//Examines the area, giving a description of it
	string result = m_ExamineText + "\n";

	//In addition, describe all present interactables
	if (m_InteractablesArr.size() > 0)
	{
		auto examine = [&](Interactable* obj) { result += obj->GetAreaExamineText() + "\n"; };
		for (auto it : m_InteractablesArr) examine(it.second);
		result += "\n";
	}

	//Tell the player the possible directions he can go
	bool canMove = m_ConnectedAreasPointersArr.size() > 0;
	if (canMove) result += "From here, it looks like you can go:";

	for (auto it : m_ConnectedAreasPointersArr)
	{
		switch (it.first)
		{
		case AREADIRECTION::NORTH:
			result += "\n-North";
			break;
		case AREADIRECTION::EAST:
			result += "\n-East";
			break;
		case AREADIRECTION::SOUTH:
			result += "\n-South";
			break;
		case AREADIRECTION::WEST:
			result += "\n-West";
			break;
		}
	}

	if (canMove) result += "\n\n";

	//Return the result
	return result;
}

void Area::PostLoad()
{
	//After loading, connect the areas with pointers instead of strings (way easier and cleaner to use)
	ConnectAreas();
}

void Area::ConnectAreas()
{
	//Add pointers for each area direction by finding the corresponding area per identifier
	for (auto area : m_ConnectedAreasArr)
	{
		m_ConnectedAreasPointersArr[area.first] = CONTENT->GetArea(area.second.m_AreaName);
		SetLocked(area.second.m_AreaDirection, area.second.m_AreaLocked);
	}

	//Clear m_ConnectedAreasArr
	m_ConnectedAreasArr.clear();
}
