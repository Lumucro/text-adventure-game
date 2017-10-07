#pragma once
#include "../stdafx.h"

class Interactable;

struct TemporaryConnection
{

public:

	TemporaryConnection(){}
	TemporaryConnection(AREADIRECTION dir, const string& name, const bool locked) : m_AreaDirection(dir), m_AreaName(name), m_AreaLocked(locked) {}

	AREADIRECTION m_AreaDirection;
	string m_AreaName = "";
	bool m_AreaLocked = false;

};

class Area final
{

public:

	Area(const string& identifierRef);
	~Area();

	//For when entering the area, sometimes we want to do like a one-time 'something falls as you enter the room' thing
	void Enter();

	//Setters
	void SetFirstEntryText(const string& text) { m_FirstEntry = true;  m_FirstEntryText = text; }
	void SetName(const string& name);
	void SetExamine(const string& text);
	void AddAmbient(const string& ambient);
	void SetLocked(AREADIRECTION dir, const bool locked);
	//Getters
	const string GetName() { return m_Name; }
	const string GetIdentifier() { return m_Identifier; }
	const vector<string> GetAmbient() { return m_AmbientVec; }
	const string GetExamineText() { return m_ExamineText; }
	const map<string, Interactable*> GetInteractablesArr() { return m_InteractablesArr; }
	Area* GetConnectedArea(AREADIRECTION area);
	const bool IsLocked(AREADIRECTION dir) { return m_LockedAreasArr[int(dir)]; }
	
	//Adding and removing interactables
	void AddInteractable(Interactable* objRef);
	bool RemoveInteractable(const string& identifierRef);
	//Setting connected areas by string
	void SetConnectedArea(AREADIRECTION dir, const TemporaryConnection& tempArea);

	//Examining an area
	const string Examine();

	//Post-load 
	void PostLoad();

protected:

	//After all areas are done loading, add the connections
	void ConnectAreas();

	//First-time entry event
	bool m_FirstEntry = false;
	string m_FirstEntryText = "";

	//Identifier
	string m_Identifier = "";
	//Name of area and examine text
	string m_Name = "";
	string m_ExamineText = "";
	//Ambient texts
	vector<string> m_AmbientVec;

	//List of present interactables
	//Use pointers if i decide to inherit from Object at any point (slicing)
	map<string, Interactable*> m_InteractablesArr;
	//List of connected areas
	map<AREADIRECTION, TemporaryConnection> m_ConnectedAreasArr;
	map<AREADIRECTION, Area*> m_ConnectedAreasPointersArr;
	//Locked or not
	vector<bool> m_LockedAreasArr = { false, false, false, false };

};
