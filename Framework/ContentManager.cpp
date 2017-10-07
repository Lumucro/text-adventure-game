#include "ContentManager.h"
#include "Classes/Area.h"
#include "Classes/Interactable.h"
#include "../Framework/File/FileReader.h"

ContentManager* ContentManager::m_Instance = nullptr;

ContentManager* ContentManager::GetInstance()
{
	if (!m_Instance)
		m_Instance = new ContentManager();

	return m_Instance;
}

ContentManager::ContentManager()
{
}

ContentManager::~ContentManager()
{
	//Clean up areas
	map<string, Area*>::iterator itArea;
	for (itArea = m_AreasMap.begin(); itArea != m_AreasMap.end(); ++itArea)
	{
		delete itArea->second;
	}
	m_AreasMap.clear();

	//Clean up objects
	map<string, Interactable*>::iterator itObjects;
	for (itObjects = m_InteractablesMap.begin(); itObjects != m_InteractablesMap.end(); ++itObjects)
	{
		delete itObjects->second;
	}
	m_InteractablesMap.clear();
}

void ContentManager::Destroy()
{
	if (m_Instance)
		delete m_Instance;
}

bool ContentManager::Load(bool debug)
{
	//Debugmode for future use
	m_DebugMode = debug;
	//Create filereader
	FileReader* input = new FileReader(m_DebugMode);
		
	//Load objects
	if (input->ReadFile("Resources/objects.txt"))
		Load<Interactable>(input, m_InteractablesMap, m_DebugMode);
	else
	{
		delete input;
		return false;
	}

	//Load areas
	if (input->ReadFile("Resources/areas.txt"))
		Load<Area>(input, m_AreasMap, m_DebugMode);
	else
	{
		delete input;
		return false;
	}

	//Delete the filereader
	delete input;

	//Post-load areas (establish actual connections between areas with pointers)
	for (auto area : m_AreasMap)
	{
		area.second->PostLoad();
	}

	//Debug to know how many areas loaded and how many objects
	if (m_DebugMode)
	{
		OUTPUT->Log("Amount of areas: " + to_string(m_AreasMap.size()) + "\n", OUTPUT_COLOR::GREEN);
		OUTPUT->Log("Amount of objects: " + to_string(m_InteractablesMap.size()) + "\n", OUTPUT_COLOR::GREEN);
	}

	return true;
}

Area* ContentManager::GetArea(const string& areaRef)
{
	//Check if the area exists
	auto it = m_AreasMap.find(areaRef);
	if (it != m_AreasMap.end())
	{
		return it->second;
	}

	if (m_DebugMode) OUTPUT->Log("Tried to get non-existant area '" + areaRef + "'!\n", OUTPUT_COLOR::RED);
	return nullptr;
}

Interactable* ContentManager::GetInteractable(const string& interactableRef)
{
	//Check if the item exists
	auto it = m_InteractablesMap.find(interactableRef);
	if (it != m_InteractablesMap.end())
	{
		return it->second;
	}

	if (m_DebugMode) OUTPUT->Log("Tried to get non-existant interactable '" + interactableRef + "'!\n", OUTPUT_COLOR::RED);
	return nullptr;
}

template <typename T>
typename enable_if<std::is_same<Interactable, T>::value || std::is_same<Area, T>::value, void>::type
ContentManager::Load(FileReader* pFileReader, map<string, T*>& objectVector, bool debug)
{
	string line = "";

	//Some variables we need to traverse through the file
	T* pTempObj = nullptr;
	string identifier = "";
	bool inObject = false;
	bool inProperty = false;

	//Values for the object (key, value, and valuevector in case there is more than one)
	string propertyKey = "";
	vector<string> propertyValues;

	//Read the file line by line
	//Large if-statement, switch not possible
	while (pFileReader->ReadLine(line))
	{
		if (line.substr(0, 2) == "//" || line == "")
		{
			//Commented line
			continue;
		}
		else if (!inObject && identifier == "")
		{
			//Identifier
			identifier = line;
		}
		else if (line == "{")
		{
			//In-object list of stuff (like descriptions)
			if (inObject)
			{
				inProperty = true;
			}
			//New object
			else
			{
				pTempObj = new T(identifier);
				inObject = true;
			}
		}
		else if (line == "}")
		{
			//End of object
			if (!inProperty && inObject && pTempObj != nullptr)
			{
				//Push back the new object
				objectVector.insert(pair<string, T*>(identifier, pTempObj));
				//Reset stuff
				inObject = false;
				identifier = "";
				pTempObj = nullptr;
			}
			else
			{
				inProperty = false;
				//Add the values to the object
				AddPropertyToObject(pTempObj, propertyKey, propertyValues, debug);
				//Clear the key and values
				propertyKey = "";
				propertyValues.clear();
			}
		}
		else
		{
			if (!inProperty)
			{
				//Key of the objects property
				propertyKey = line.substr(0, line.find("="));

				//If there is no =, assume a list will follow
				//If there is, it's a single-val property indeed
				if (line.find("=") != line.npos)
				{
					//Value
					propertyValues.push_back(line.substr(line.find("=") + 1));
					//Add the property to the object
					AddPropertyToObject(pTempObj, propertyKey, propertyValues, debug);
					//Clear the key and values
					propertyKey = "";
					propertyValues.clear();
				}
			}
			else
			{
				//A new property, as part of the key (more than 1 property), line is the entire thing
				propertyValues.push_back(line);
			}
		}
	}
}

bool ContentManager::AddPropertyToObject(Interactable* pObject, const string& keyRef, const vector<string>& valuesRef, bool debug)
{
	if (debug)
	{
		OUTPUT->Log("Adding key " + keyRef + " to object\n");
		for (string val : valuesRef)
			OUTPUT->Log("\tvalue: " + val + "\n");
	}
	
	//This is basically another long if statement, but it gets the job done
	//Alternatively at some point I should give objects and areas a std::map with keys and values, that'd be way cleaner and easier

	//Name
	if (keyRef == "name")
	{
		pObject->SetName(valuesRef[0]);
	}
	//Identifiers
	else if (keyRef == "identifiers")
	{
		for (auto val : valuesRef)
		{
			pObject->AddIdentifier(val);
		}
	}
	//Area examine (will be added to the examine text of an area, eg: You're in a dark room. There's a rock.)
	else if (keyRef == "examine")
	{
		pObject->SetAreaExamineText(valuesRef[0]);
	}
	//What does the player find when interacting with this
	//For getting objects from other objects (examine rock: find a key)
	else if (keyRef == "pickup")
	{
		pObject->SetPickupItem(valuesRef[0]);
	}
	//Pickup text for other items
	else if (keyRef == "pickuptext")
	{
		pObject->SetPickupText(valuesRef[0]);
	}
	//Already picked up text
	else if (keyRef == "alreadypickedup")
	{
		pObject->SetAlreadyPickedUptext(valuesRef[0]);
	}
	//Can this item itself be picked up? (pick up the rock)
	else if (keyRef == "canpickup")
	{
		string val = valuesRef[0];
		bool canpickup = (val == "1") ? true : false;
		pObject->SetCanPickup(canpickup);
	}
	//What this can be used on (key on lock)
	else if (keyRef == "useon")
	{
		pObject->SetUseItem(valuesRef[0]);
	}
	//Use type (0 = kill player, 1 = unlocks direction)
	else if (keyRef == "usetype")
	{
		INTERACTABLE_USETYPE type = INTERACTABLE_USETYPE::NOTHING;

		//Switch for type
		switch (valuesRef[0][0])
		{
		case '1':
			type = INTERACTABLE_USETYPE::DEATH;
			break;
		case '2':
			type = INTERACTABLE_USETYPE::UNLOCK;
			break;
		case '3':
			type = INTERACTABLE_USETYPE::WIN;
			break;
		}

		//Set the type
		pObject->SetUseType(type);
	}
	//Text for when using success
	else if (keyRef == "usetext")
	{
		pObject->SetUseText(valuesRef[0]);
	}
	//Text for when using fails
	else if (keyRef == "usefailtext")
	{
		pObject->SetUseFailText(valuesRef[0]);
	}
	//Descriptions (can be multiple)
	else if (keyRef == "desc")
	{
		for (auto val : valuesRef)
		{
			pObject->AddDescription(val);
		}
	}

	//Unlocks (for keys and whatnot)
	else if (keyRef == "unlocks")
	{
		AREADIRECTION dir = AREADIRECTION::NONE;

		//Switch for direction
		switch (valuesRef[0][0])
		{
		case 'n':
			dir = AREADIRECTION::NORTH;
			break;
		case 'e':
			dir = AREADIRECTION::EAST;
			break;
		case 's':
			dir = AREADIRECTION::SOUTH;
			break;
		case 'w':
			dir = AREADIRECTION::WEST;
			break;
		}

		//Set direction
		pObject->SetUnlockDirection(dir);
	}

	return false;
}

bool ContentManager::AddPropertyToObject(Area* pObject, const string& keyRef, const vector<string>& valuesRef, bool debug)
{
	if (debug)
	{
		OUTPUT->Log("Adding key " + keyRef + " to area\n");
		for (string val : valuesRef)
			OUTPUT->Log("\tvalue: " + val + "\n");
	}

	//This is basically another long if statement, but it gets the job done
	//Alternatively at some point I should give objects and areas a std::map with keys and values, that'd be way cleaner and easier

	//Name, only one
	if (keyRef == "name")
	{
		pObject->SetName(valuesRef[0]);
	}
	//Description, only one
	else if (keyRef == "desc")
	{
		pObject->SetExamine(valuesRef[0]);
	}
	//Ambient text, can be multiple
	else if (keyRef == "ambient")
	{
		for (auto val : valuesRef)
		{
			pObject->AddAmbient(val);
		}
	}
	//Connected areas
	else if (keyRef == "north" || keyRef == "east" || keyRef == "south" || keyRef == "west")
	{
		bool locked = false;
		if (valuesRef[0][0] == '!')
		{
			locked = true;
		}

		//Switch for direction
		AREADIRECTION direction;
		switch (keyRef[0])
		{
		case 'n':
			direction = AREADIRECTION::NORTH;
			break;
		case 'e':
			direction = AREADIRECTION::EAST;
			break;
		case 's':
			direction = AREADIRECTION::SOUTH;
			break;
		case 'w':
			direction = AREADIRECTION::WEST;
			break;
		}

		//Add connection
		AddAreaConnection(pObject, direction, locked ? valuesRef[0].substr(1) : valuesRef[0], locked);
	}
	//Interactables in an area
	else if (keyRef == "objects")
	{
		for (auto val : valuesRef)
		{
			//Interestingly enough (I didn't know this), doing map[key] on a non-existant key adds it to the map
			//So do a quick check to avoid this
			if (m_InteractablesMap.find(val) != m_InteractablesMap.end())
			{
				 bool success = AddInteractableToArea(m_InteractablesMap[val], pObject);			
				 if (debug && !success) OUTPUT->Log("Failed to add interactable to area (" + val + ")\n", OUTPUT_COLOR::RED);
			}
			else
			{
				if (debug) OUTPUT->Log("Non-existant interactable found in area (" + val + ")\n", OUTPUT_COLOR::RED);
			}
		}
	}
	//First enter text
	else if (keyRef == "firstenter")
	{
		pObject->SetFirstEntryText(valuesRef[0]);
	}

	return false;
}

bool ContentManager::AddInteractableToArea(Interactable* pInteractable, Area* pArea)
{
	//Check if valid
	if (pInteractable == nullptr || pArea == nullptr)
	{
		return false;
	}

	pArea->AddInteractable(pInteractable);
	return true;
}

bool ContentManager::AddAreaConnection(Area* pTargetArea, AREADIRECTION dir, const string& toConnect, const bool locked)
{
	//Check if valid
	if (pTargetArea == nullptr || toConnect == "")
	{
		return false;
	}

	if (m_DebugMode) OUTPUT->Log("Adding area connection: " + toConnect + "\n");

	//Struct with the info
	TemporaryConnection tempCon(dir, toConnect, locked);

	pTargetArea->SetConnectedArea(dir, tempCon);
	return true;
}
